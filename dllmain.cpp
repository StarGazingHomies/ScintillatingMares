#include "pch.h"
#include "Logger.h"
#include "Keyboard/KeyboardClass.h"
#include "Mouse/MouseClass.h"
#include "Graphics/Graphics.h"
#include "Path.h"
#include "LoE/FileManager.h"
#include <string>
#include <Windows.h>
#include <WinUser.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <dwrite.h>
#include <CommCtrl.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "DirectXTK.lib")
#pragma comment (lib, "comctl32")

#define safe_release(p) if (p) { p->Release(); p = nullptr; } 

#include "D3D_VMT_Indices.h"
#define VMT_PRESENT (UINT)IDXGISwapChainVMT::Present
constexpr auto PRESENT_STUB_SIZE = 5;

// d3d11 related object ptrs
using namespace DirectX;

Graphics graphics;
ID3D11VertexShader* pVertexShader = nullptr;
ID3D11InputLayout* pVertexLayout = nullptr;
ID3D11PixelShader* pPixelShader = nullptr;
ID3D11Buffer* pVertexBuffer = nullptr;
ID3D11Buffer* pIndexBuffer = nullptr;
ID3D11Buffer* pConstantBuffer = nullptr;

ID3D11Device* pDevice = nullptr;
IDXGISwapChain* pSwapchain = nullptr;
ID3D11DeviceContext* pContext = nullptr;
ID3D11RenderTargetView* pRenderTargetView = nullptr;

HRESULT __stdcall hkPresent(IDXGISwapChain* pThis, UINT SyncInterval, UINT Flags);
using fnPresent = HRESULT(__stdcall*)(IDXGISwapChain* pThis, UINT SyncInterval, UINT Flags);
void* ogPresent;					// Pointer to the original Present function
fnPresent ogPresentTramp;			// Function pointer that calls the Present stub in our trampoline
void* pTrampoline = nullptr;		// Pointer to jmp instruction in our trampoline that leads to hkPresent
char ogBytes[PRESENT_STUB_SIZE];	// Buffer to store original bytes from Present

bool modEnabled = false;

bool Hook(void* pSrc, void* pDst, size_t size);
bool WriteMem(void* pDst, char* pBytes, size_t size);
bool HookD3D();
bool CompileShader(const char* szShader, const char* szEntrypoint, const char* szTarget, ID3D10Blob** pBlob);
bool InitD3DHook(IDXGISwapChain* pSwapchain);
void CleanupD3D();
void Render();

HWND FindMainWindow(DWORD dwPID);
BOOL CALLBACK EnumWindowsCallback(HWND hWnd, LPARAM lParam);


bool Hook(void* pSrc, void* pDst, size_t size)
{
	DWORD dwOld;
	uintptr_t src = (uintptr_t)pSrc;
	uintptr_t dst = (uintptr_t)pDst;

	if (!VirtualProtect(pSrc, size, PAGE_EXECUTE_READWRITE, &dwOld))
		return false;

	*(char*)src = (char)0xE9;
	*(int*)(src + 1) = (int)(dst - src - 5);

	VirtualProtect(pSrc, size, dwOld, &dwOld);
	return true;
}

bool WriteMem(void* pDst, char* pBytes, size_t size)
{
	DWORD dwOld;
	if (!VirtualProtect(pDst, size, PAGE_EXECUTE_READWRITE, &dwOld))
		return false;

	memcpy(pDst, pBytes, PRESENT_STUB_SIZE);

	VirtualProtect(pDst, size, dwOld, &dwOld);
	return true;
}

bool HookD3D()
{
	// Create a dummy device, get swapchain vmt, hook present.
	D3D_FEATURE_LEVEL featLevel;
	DXGI_SWAP_CHAIN_DESC sd{ 0 };
	sd.BufferCount = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.Height = 800;
	sd.BufferDesc.Width = 600;
	sd.BufferDesc.RefreshRate = { 60, 1 };
	sd.OutputWindow = GetForegroundWindow();
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_REFERENCE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &sd, &pSwapchain, &pDevice, &featLevel, nullptr);
	if (FAILED(hr)) {
		printf("Failed to create D3D11 Device");
		return false;
	}

	// Get swapchain vmt
	void** pVMT = *(void***)pSwapchain;

	// Get Present's address out of vmt
	ogPresent = (fnPresent)(pVMT[VMT_PRESENT]);

	// got what we need, we can release device and swapchain now
	// we'll be stealing the game's.
	safe_release(pSwapchain);
	safe_release(pDevice);

	// Create a code cave to trampoline to our hook
	// We'll try to do this close to present to make sure we'll be able to use a 5 byte jmp (important for x64)
	void* pLoc = (void*)((uintptr_t)ogPresent - 0x2000);
	void* pTrampLoc = nullptr;
	while (!pTrampLoc)
	{
		pTrampLoc = VirtualAlloc(pLoc, 1, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		pLoc = (void*)((uintptr_t)pLoc + 0x200);
	}
	ogPresentTramp = (fnPresent)pTrampLoc;

	// write original bytes to trampoline
	// write jmp to hook
	memcpy(ogBytes, ogPresent, PRESENT_STUB_SIZE);
	memcpy(pTrampLoc, ogBytes, PRESENT_STUB_SIZE);

	pTrampLoc = (void*)((uintptr_t)pTrampLoc + PRESENT_STUB_SIZE);

	// write the jmp back into present
	*(char*)pTrampLoc = (char)0xE9;
	pTrampLoc = (void*)((uintptr_t)pTrampLoc + 1);
	uintptr_t ogPresRet = (uintptr_t)ogPresent + 5;
	*(int*)pTrampLoc = (int)(ogPresRet - (uintptr_t)pTrampLoc - 4);

	// write the jmp to our hook
	pTrampoline = pTrampLoc = (void*)((uintptr_t)pTrampLoc + 4);
#ifdef _WIN64
	// if x64, gazzillion byte absolute jmp
	char pJmp[] = { 0xFF, 0x25, 0x00, 0x00, 0x00, 0x00 };
	WriteMem(pTrampLoc, pJmp, ARRAYSIZE(pJmp));
	pTrampLoc = (void*)((uintptr_t)pTrampLoc + ARRAYSIZE(pJmp));
	*(uintptr_t*)pTrampLoc = (uintptr_t)hkPresent;
#else
	// if x86, normal 0xE9 jmp
	* (char*)pTrampLoc = (char)0xE9;
	pTrampLoc = (void*)((uintptr_t)pTrampLoc + 1);
	*(int*)pTrampLoc = (uintptr_t)hkPresent - (uintptr_t)pTrampLoc - 4;
#endif

	// hook present, place a normal mid-function at the beginning of the Present function
	return Hook(ogPresent, pTrampoline, PRESENT_STUB_SIZE);
}

bool CompileShader(const char* szShader, const char* szEntrypoint, const char* szTarget, ID3D10Blob** pBlob) {
	ID3D10Blob* pErrorBlob = nullptr;

	HRESULT hr = D3DCompile(szShader, strlen(szShader), 0, nullptr, nullptr, szEntrypoint, szTarget, D3DCOMPILE_ENABLE_STRICTNESS, 0, pBlob, &pErrorBlob);
	if (FAILED(hr))
	{
		if (pErrorBlob)
		{
			char szError[256]{ 0 };
			memcpy(szError, pErrorBlob->GetBufferPointer(), pErrorBlob->GetBufferSize());
			MessageBoxA(nullptr, szError, "Error", MB_OK);
		}
		return false;
	}
	return true;
}


// Direct3D Init Function
// (Only initialize once to avoid memory leaks!)
bool initialized = false;
FileManager fm;
bool InitD3DHook(IDXGISwapChain* pSwapchain) {
	if (initialized) return true;

	graphics.Initialize(pSwapchain);
	fm = FileManager();

	initialized = true;

	return true;
}

void CleanupD3D()
{
	safe_release(pVertexBuffer);
	safe_release(pIndexBuffer);
	safe_release(pConstantBuffer);
	safe_release(pPixelShader);
	safe_release(pVertexShader);
	safe_release(pVertexLayout);
}

KeyboardClass keyboard = KeyboardClass();
MouseClass mouse = MouseClass();
void Render()
{
	// Make sure input works!
	while (!keyboard.keyBufferIsEmpty()) {
		KeyboardEvent event = keyboard.readKey();
		printf("%ls\n", event.toString().c_str());
		if (event.getKeyCode() == VK_F6 && event.isPress()) {
			modEnabled = !modEnabled;
		}
	}

	while (!mouse.eventBufferIsEmpty()) {
		MouseEvent event = mouse.readEvent();
		printf("%ls\n", event.toString().c_str());
	}

	if (modEnabled) {
		graphics.RenderFrame();
	}
}

HRESULT __stdcall hkPresent(IDXGISwapChain* pThis, UINT SyncInterval, UINT Flags)
{
	pSwapchain = pThis;

	if (!pDevice)
	{
		if (!InitD3DHook(pThis))
			return false;
	}

	Render();
	return ogPresentTramp(pThis, SyncInterval, Flags);
}

HWND FindMainWindow(DWORD dwPID)
{
	HandleData handleData{ 0 };
	handleData.pid = dwPID;
	EnumWindows(EnumWindowsCallback, (LPARAM)&handleData);
	return handleData.hWnd;
}

BOOL CALLBACK EnumWindowsCallback(HWND hWnd, LPARAM lParam)
{
	HandleData& data = *(HandleData*)lParam;
	DWORD pid = 0;
	GetWindowThreadProcessId(hWnd, &pid);
	if (pid == data.pid && GetWindow(hWnd, GW_OWNER) == HWND(0) && IsWindowVisible(hWnd))
	{
		data.hWnd = hWnd;
		return FALSE;
	}

	return TRUE;
}

void ReadFromMemory(DWORD addressToRead, float value) {
	value = *(float*)addressToRead;
}

// Callback function
WNDPROC OldWndProc;
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

//LRESULT CALLBACK NewWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
LRESULT CALLBACK NewWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	// ImGui handle inputs
	if (modEnabled && ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam)) {
		return true;
	}

	switch (uMsg) {
	case WM_KEYDOWN: {
		unsigned char keycode = static_cast<unsigned char>(wParam);
		if (keyboard.isKeysAutoRepeat())
		{
			keyboard.onKeyPressed(keycode, lParam);
		}
		else
		{
			const bool wasPressed = lParam & (1<<30);
			if (!wasPressed)
			{
				keyboard.onKeyPressed(keycode, lParam);
			}
		}
		if (modEnabled) return 0;
		else return OldWndProc(hwnd, uMsg, wParam, lParam);
	}
	case WM_KEYUP: {
		unsigned char keycode = static_cast<unsigned char>(wParam);
		keyboard.onKeyReleased(keycode, lParam);
		if (modEnabled) return 0;
		else return OldWndProc(hwnd, uMsg, wParam, lParam);
	}
	case WM_CHAR: {
		unsigned char ch = static_cast<unsigned char>(wParam);
		if (keyboard.isCharsAutoRepeat())
		{
			keyboard.onChar(ch);
		}
		else
		{
			const bool wasPressed = lParam & (1<<30);
			if (!wasPressed)
			{
				keyboard.onChar(ch);
			}
		}
		if (modEnabled) return 0;
		else return OldWndProc(hwnd, uMsg, wParam, lParam);
	}
	case WM_MOUSEMOVE: {
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		mouse.onMouseMove(x, y);
		if (modEnabled) return 0;
		else return OldWndProc(hwnd, uMsg, wParam, lParam);
	}
	case WM_LBUTTONDOWN: {
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		mouse.onLeftPressed(x, y);
		if (modEnabled) return 0;
		else return OldWndProc(hwnd, uMsg, wParam, lParam);
	}
	case WM_LBUTTONUP: {
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		mouse.onLeftReleased(x, y);
		if (modEnabled) return 0;
		else return OldWndProc(hwnd, uMsg, wParam, lParam);
	}
	case WM_RBUTTONDOWN: {
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		mouse.onRightPressed(x, y);
		if (modEnabled) return 0;
		else return OldWndProc(hwnd, uMsg, wParam, lParam);
	}
	case WM_RBUTTONUP: {
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		mouse.onRightReleased(x, y);
		if (modEnabled) return 0;
		else return OldWndProc(hwnd, uMsg, wParam, lParam);
	}
	case WM_MBUTTONDOWN: {
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		mouse.onMiddlePressed(x, y);
		if (modEnabled) return 0;
		else return OldWndProc(hwnd, uMsg, wParam, lParam);
	}
	case WM_MBUTTONUP: {
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		mouse.onMiddleReleased(x, y);
		if (modEnabled) return 0;
		else return OldWndProc(hwnd, uMsg, wParam, lParam);
	}
	case WM_MOUSEWHEEL: {
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		short wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		if (wheelDelta > 0) {
			mouse.onWheelUp(x, y);
		}
		else {
			mouse.onWheelDown(x, y);
		}
		if (modEnabled) return 0;
		else return OldWndProc(hwnd, uMsg, wParam, lParam);
	}
	default: {}
	}
	return OldWndProc(hwnd, uMsg, wParam, lParam);
}

const DWORD_PTR funcOffset = 0x59C980;
// This is the offset for the Mining::getInstance() function iirc
typedef long long* (*UnExportedFunc)();

long long* CallUnExportedFunc() {
	// This will get the DLL base address (which can vary)
	HMODULE hMod = GetModuleHandleW(L"GameAssembly.dll");
	// Calcualte the acutal address 
	DWORD_PTR funcAddress = (DWORD_PTR)hMod + funcOffset;
	// Cast the address to a function poniter
	UnExportedFunc func = (UnExportedFunc)funcAddress;
	// Call the function
	return func();
}

DWORD WINAPI MainThread(LPVOID param) {
    // Console window
    AllocConsole();
    FILE* f = new FILE();
	freopen_s(&f, "CONOUT$", "w", stdout);
	printf("DLL Initialized!\n");

	bool result = HookD3D();
	if (result) {
		printf("D3D successfully hooked.\n");
		HMODULE hMod = GetModuleHandleW(L"GameAssembly.dll");
		printf("GameAssembly dll location: %lld\n", (DWORD_PTR)hMod);
		
		// Not ready for this yet :/
		//printf("%d\n", CallUnExportedFunc());
		while (true) {
			Sleep(100);
		}
	}
	else {
		printf("D3D did not successfully hook. Stopping program.\n");
	}

	printf("Program ended. Closing the console will no longer send SIGINT to Legends of Equestria.\n");
	
	FreeConsole();

	// Cleanup and unload dll
	CleanupD3D();
	WriteMem(ogPresent, ogBytes, PRESENT_STUB_SIZE);
	VirtualFree((void*)ogPresentTramp, 0x1000, MEM_RELEASE);
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)FreeLibrary, param, 0, 0);

    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH: {
		DisableThreadLibraryCalls(hModule);
		// MessageBoxA(NULL, "DLL Injected!", "Mares Mares Mares!", MB_OK);
		// Logger::Log(E_INVALIDARG, "Test Message");
		// Get the current window so we can subclass WndProc

		HWND hWnd = FindWindow(NULL, TEXT("Legends of Equestria"));
		//Logger::Log("Window process handle: " + std::to_string((int)hWnd));
		if (hWnd) {
			//bool subclassResult = SetWindowSubclass(hWnd, &NewWindowProc, MY_SUBCLASS_ID, 0);
			//Logger::Log("Window subclass result: " + std::to_string(subclassResult));
			OldWndProc = (WNDPROC) SetWindowLongPtr(hWnd, -4, (LONG_PTR)NewWindowProc);
			//Logger::Log("Window subclass result: " + std::to_string((int)OldWndProc));
			DWORD err = GetLastError();
			//Logger::Log(err, "Subclass error");
		}

        HANDLE hThread = CreateThread(0, 0, MainThread, hModule, 0, 0);
        if (hThread != nullptr)
            CloseHandle(hThread);
        }
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return true;
}


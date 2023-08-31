#include "pch.h"
#include "Logger.h"
#include "KeyboardClass.h"
#include "MouseClass.h"
#include "Graphics.h"
#include "Path.h"
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

#include "shaders.h"
#include "D3D_VMT_Indices.h"
#define VMT_PRESENT (UINT)IDXGISwapChainVMT::Present
#define PRESENT_STUB_SIZE 5

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

// Changing this to an array of viewports
#define MAINVP 0
D3D11_VIEWPORT pViewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE]{ 0 };
XMMATRIX mOrtho;

struct ConstantBuffer
{
	XMMATRIX mProjection;
};

struct Vertex
{
	XMFLOAT3 pos;
	XMFLOAT4 color;
};

HRESULT __stdcall hkPresent(IDXGISwapChain* pThis, UINT SyncInterval, UINT Flags);
using fnPresent = HRESULT(__stdcall*)(IDXGISwapChain* pThis, UINT SyncInterval, UINT Flags);
void* ogPresent;					// Pointer to the original Present function
fnPresent ogPresentTramp;			// Function pointer that calls the Present stub in our trampoline
void* pTrampoline = nullptr;		// Pointer to jmp instruction in our trampoline that leads to hkPresent
char ogBytes[PRESENT_STUB_SIZE];	// Buffer to store original bytes from Present

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
bool InitD3DHook(IDXGISwapChain* pSwapchain) {

	graphics.Initialize(pSwapchain);

	//// initialize shaders
	//ID3D10Blob* pBlob = nullptr;

	//// create vertex shader
	//if (!CompileShader(simpleShaders, "VS", "vs_5_0", &pBlob))
	//	return false;

	//hr = pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader);
	//if (FAILED(hr))
	//	return false;

	//// Define/create the input layout for the vertex shader
	//D3D11_INPUT_ELEMENT_DESC layout[2] = {
	//{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	//{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	//};
	//UINT numElements = ARRAYSIZE(layout);

	//hr = pDevice->CreateInputLayout(layout, numElements, pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &pVertexLayout);
	//if (FAILED(hr))
	//	return false;

	//safe_release(pBlob);

	//// create pixel shader
	//if (!CompileShader(simpleShaders, "PS", "ps_5_0", &pBlob))
	//	return false;

	//hr = pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader);
	//if (FAILED(hr))
	//	return false;

	//UINT numViewports = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
	//float fWidth = 0;
	//float fHeight = 0;

	//// Apparently this isn't universal. Works on some games
	//pContext->RSGetViewports(&numViewports, pViewports);

	//if (!numViewports || !pViewports[MAINVP].Width)
	//{
	//	// This should be retrieved dynamically
	//	//HWND hWnd0 = FindWindowA( "W2ViewportClass", nullptr );
	//	HWND hWnd = FindMainWindow(GetCurrentProcessId());
	//	RECT rc{ 0 };
	//	if (!GetClientRect(hWnd, &rc))
	//		return false;

	//	fWidth = (float)rc.right;
	//	fHeight = (float)rc.bottom;

	//	// Setup viewport
	//	pViewports[MAINVP].Width = (float)fWidth;
	//	pViewports[MAINVP].Height = (float)fHeight;
	//	pViewports[MAINVP].MinDepth = 0.0f;
	//	pViewports[MAINVP].MaxDepth = 1.0f;

	//	// Set viewport to context
	//	pContext->RSSetViewports(1, pViewports);
	//}
	//else
	//{
	//	fWidth = (float)pViewports[MAINVP].Width;
	//	fHeight = (float)pViewports[MAINVP].Height;
	//}

	//// Create the constant buffer
	//D3D11_BUFFER_DESC bd{ 0 };
	//bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	//bd.ByteWidth = sizeof(ConstantBuffer);
	//bd.Usage = D3D11_USAGE_DEFAULT;

	//// Setup orthographic projection
	//mOrtho = XMMatrixOrthographicLH(fWidth, fHeight, 0.0f, 1.0f);
	//ConstantBuffer cb;
	//cb.mProjection = mOrtho;

	//D3D11_SUBRESOURCE_DATA sr{ 0 };
	//sr.pSysMem = &cb;
	//hr = pDevice->CreateBuffer(&bd, &sr, &pConstantBuffer);
	//if (FAILED(hr))
	//	return false;

	//// Create a quad
	//// Create a vertex buffer, start by setting up a description.
	//ZeroMemory(&bd, sizeof(bd));
	//bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//bd.Usage = D3D11_USAGE_DEFAULT;
	//bd.ByteWidth = 4 * sizeof(Vertex);
	//bd.StructureByteStride = sizeof(Vertex);

	//// left and top edge of window
	//float left = fWidth / -2;
	//float top = fHeight / 2;

	//// Width and height of triangle
	//float w = 500;
	//float h = 500;

	//// Center position of triangle, this should center it in the screen.
	//float fPosX = -1 * left;
	//float fPosY = top;

	//// Setup vertices of triangle
	//Vertex pVerts[4] = {
	//	{ XMFLOAT3(left + fPosX + w / 2,	top - fPosY + h / 2,	1.0f),	XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) },
	//	{ XMFLOAT3(left + fPosX - w / 2,	top - fPosY + h / 2,	1.0f),	XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
	//	{ XMFLOAT3(left + fPosX + w / 2,	top - fPosY - h / 2,	1.0f),	XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
	//	{ XMFLOAT3(left + fPosX - w / 2,	top - fPosY - h / 2,	1.0f),	XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) },
	//};

	//// create the buffer.
	//ZeroMemory(&sr, sizeof(sr));
	//sr.pSysMem = &pVerts;
	//hr = pDevice->CreateBuffer(&bd, &sr, &pVertexBuffer);
	//if (FAILED(hr))
	//	return false;

	//// Create an index buffer
	//ZeroMemory(&bd, sizeof(bd));
	//ZeroMemory(&sr, sizeof(sr));

	//UINT pIndices[6] = { 0, 1, 2, 1, 2, 3 };
	//bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	//bd.Usage = D3D11_USAGE_DEFAULT;
	//bd.ByteWidth = sizeof(UINT) * 6;
	//bd.StructureByteStride = sizeof(UINT);

	//sr.pSysMem = &pIndices;
	//hr = pDevice->CreateBuffer(&bd, &sr, &pIndexBuffer);
	//if (FAILED(hr))
	//	return false;

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
	graphics.RenderFrame();
	//// Make sure input works!
	//while (!keyboard.keyBufferIsEmpty()) {
	//	KeyboardEvent event = keyboard.readKey();
	//	printf("%ls\n", event.toString().c_str());
	//}

	//while (!mouse.eventBufferIsEmpty()) {
	//	MouseEvent event = mouse.readEvent();
	//	printf("%ls\n", event.toString().c_str());
	//}

	//// Make sure our render target is set.
	//pContext->OMSetRenderTargets(1, &pRenderTargetView, nullptr);

	//// Update view
	//ConstantBuffer cb;
	//cb.mProjection = XMMatrixTranspose(mOrtho);
	//pContext->UpdateSubresource(pConstantBuffer, 0, nullptr, &cb, 0, 0);
	//pContext->VSSetConstantBuffers(0, 1, &pConstantBuffer);

	//// Make sure the input assembler knows how to process our verts/indices
	//UINT stride = sizeof(Vertex);
	//UINT offset = 0;
	//pContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
	//pContext->IASetInputLayout(pVertexLayout);
	//pContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	//pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//// Set the shaders we need to render our triangle
	//pContext->VSSetShader(pVertexShader, nullptr, 0);
	//pContext->PSSetShader(pPixelShader, nullptr, 0);

	//// Set viewport to context
	//pContext->RSSetViewports(1, pViewports);

	//// Draw triangle
	//if (keyboard.keyIsPressed(VK_F6)) {
	//	pContext->DrawIndexed(6, 0, 0);
	//}
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
//LRESULT CALLBACK NewWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
LRESULT CALLBACK NewWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
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
		return 0;
	}
	case WM_KEYUP: {
		unsigned char keycode = static_cast<unsigned char>(wParam);
		keyboard.onKeyReleased(keycode, lParam);
		return 0;
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
		return 0;
	}
	case WM_MOUSEMOVE: {
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		mouse.onMouseMove(x, y);
		return 0;
	}
	case WM_LBUTTONDOWN: {
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		mouse.onLeftPressed(x, y);
		return 0;
	}
	case WM_LBUTTONUP: {
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		mouse.onLeftReleased(x, y);
		return 0;
	}
	case WM_RBUTTONDOWN: {
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		mouse.onRightPressed(x, y);
		return 0;
	}
	case WM_RBUTTONUP: {
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		mouse.onRightReleased(x, y);
		return 0;
	}
	case WM_MBUTTONDOWN: {
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		mouse.onMiddlePressed(x, y);
		return 0;
	}
	case WM_MBUTTONUP: {
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		mouse.onMiddleReleased(x, y);
		return 0;
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
		return 0;
	}
	default:
		return OldWndProc(hwnd, uMsg, wParam, lParam);
	}
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
		while (true) {
			//if (GetAsyncKeyState(VK_F6) & 0x80000) {
			//	MessageBoxA(NULL, "F6 PRESSED!", "F6 PRESSED!", MB_OK);
			//}
			if (keyboard.keyIsPressed(VK_F7) & 0x80000) {
				break;
			}
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


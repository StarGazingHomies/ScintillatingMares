#include "pch.h"
#include "Graphics.h"

bool Graphics::LinkDirectX(IDXGISwapChain* pSwapchain) {
	this->pSwapchain = pSwapchain;

	HRESULT hr = pSwapchain->GetDevice(__uuidof(ID3D11Device), (void**)&this->pDevice);
	if (FAILED(hr)) {
		Logger::Log(hr, "Failed to get D3D device");
		return false;
	}

	this->pDevice->GetImmediateContext(&this->pContext);
	this->pContext->OMGetRenderTargets(1, &this->pRenderTargetView, nullptr);

	// If for some reason we fail to get a render target, create one.
	// This will probably never happen with a real game but maybe certain test environments... :P
	if (!pRenderTargetView)
	{
		// Get a pointer to the back buffer for the render target view
		ID3D11Texture2D* pBackbuffer = nullptr;
		hr = pSwapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackbuffer));
		if (FAILED(hr)) {
			Logger::Log(hr, "Failed to get back buffer");
			return false;
		}

		// Create render target view
		hr = pDevice->CreateRenderTargetView(pBackbuffer, nullptr, &pRenderTargetView);
		pBackbuffer->Release();
		if (FAILED(hr)) {
			Logger::Log(hr, "Failed to create render target view");
			return false;
		}

		// Make sure our render target is set.
		pContext->OMSetRenderTargets(1, &pRenderTargetView, nullptr);
	}

	// Yoink the width and height from viewport.
	// We don't have to set it up, we just have to get the existing one.

	UINT numViewports = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
#define MAINVP 0
	D3D11_VIEWPORT pViewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE]{ 0 };
	// Apparently this isn't universal. Works on some games
	pContext->RSGetViewports(&numViewports, pViewports);

	if (!numViewports || !pViewports[MAINVP].Width)
	{
		// This should be retrieved dynamically
		//HWND hWnd0 = FindWindowA( "W2ViewportClass", nullptr );
		HWND hWnd = FindMainWindow(GetCurrentProcessId());
		RECT rc{ 0 };
		if (!GetClientRect(hWnd, &rc))
			return false;

		width = (float)rc.right;
		height = (float)rc.bottom;

		// Setup viewport
		pViewports[MAINVP].Width = (float)width;
		pViewports[MAINVP].Height = (float)height;
		pViewports[MAINVP].MinDepth = 0.0f;
		pViewports[MAINVP].MaxDepth = 1.0f;

		// Set viewport to context
		pContext->RSSetViewports(1, pViewports);
	}
	else
	{
		width = (float)pViewports[MAINVP].Width;
		height = (float)pViewports[MAINVP].Height;
	}

	return true;
}

bool Graphics::Initialize(IDXGISwapChain* pSwapchain) {
	if (!LinkDirectX(pSwapchain)) return false;
	if (!InitializeShaders()) return false;
	if (!InitializeScene()) return false;
	return true;
}

void Graphics::RenderFrame() {
	float bgcolor[] = { 0.0f, 0.0f, 1.0f, 1.0f };
	this->pContext->ClearRenderTargetView(this->pRenderTargetView, bgcolor);

	this->pContext->IASetInputLayout(this->vertexShader.getInputLayout());
	this->pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);

	this->pContext->VSSetShader(vertexShader.getShader(), NULL, 0);
	this->pContext->PSSetShader(pixelShader.getShader(), NULL, 0);

	UINT stride = sizeof(Vertex2D);
	UINT offset = 0;
	this->pContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);

	this->pContext->Draw(1, 0);

	// Don't call present() since we are hooked into the beginning of present.
	// Calling it creates infinite loop    *derp*
	//this->pSwapchain->Present(1, NULL);
}

bool Graphics::InitializeShaders() {

	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{
			"POSITION", // Name
			0,  // ID
			DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, // Format
			0,  // Input slot
			0,  // Offset
			D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, // Vertex or instance data
			0   // Instance data separate 
		}
	};

	UINT numElements = ARRAYSIZE(layout);

	if (!vertexShader.Initialize(pDevice, L"vertexshader.cso", layout, numElements)) return false;

	if (!pixelShader.Initialize(pDevice, L"pixelshader.cso", layout, numElements)) return false;

	return true;
}

bool Graphics::InitializeScene()
{
	Vertex2D v[] = {
		Vertex2D(0.0f, 0.0f)
	};

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex2D) * ARRAYSIZE(v);
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;
	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	vertexBufferData.pSysMem = v;

	HRESULT hr = this->pDevice->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &this->pVertexBuffer);
	if (FAILED(hr)) {
		Logger::Log(hr, "Failed to create render vertex buffer");
		return false;
	}

	return true;
}

HWND Graphics::FindMainWindow(DWORD dwPID) {
	HandleData handleData{ 0 };
	handleData.pid = dwPID;
	EnumWindows(EnumWindowsCallback, (LPARAM)&handleData);
	return handleData.hWnd;
}

BOOL Graphics::EnumWindowsCallback(HWND hWnd, LPARAM lParam) {
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

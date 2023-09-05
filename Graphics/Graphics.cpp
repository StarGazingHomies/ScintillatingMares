#include "../pch.h"
#include "Graphics.h"

bool Graphics::Initialize(IDXGISwapChain* pSwapchain) {
	if (!LinkDirectX(pSwapchain)) return false;
	if (!InitializeObjects()) return false;
	if (!InitDirectX()) return false;
	if (!InitializeShaders()) return false;
	if (!InitializeScene()) return false;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	HWND hwnd = FindMainWindow(GetCurrentProcessId());
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(pDevice, pContext);
	ImGui::StyleColorsDark();

	return true;
}

void Graphics::RenderFrame() {
	this->pContext->OMSetRenderTargets(1, &this->pRenderTargetView, nullptr);
	this->pContext->RSSetViewports(1, this->pViewports);

	float bgcolor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	this->pContext->ClearRenderTargetView(this->pRenderTargetView, bgcolor);

	// Input layout
	this->pContext->IASetInputLayout(this->vertexShader.getInputLayout());
	this->pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Rasterizer state
	//this->pContext->RSSetState(this->pRasterizerState);

	// Shaders
	this->pContext->VSSetShader(this->vertexShader.getShader(), NULL, 0);
	this->pContext->PSSetShader(this->pixelShader.getShader(), NULL, 0);

	// Texture
	this->pContext->PSSetSamplers(0, 1, &this->samplerState);
	this->pContext->PSSetShaderResources(0, 1, &this->itemsTexture);

	// Update constant buffer
	constantBuffer.data.xOffset = 0.0f;
	constantBuffer.data.yOffset = 0.5f;
	if (!constantBuffer.applyChanges()) return;

	this->pContext->VSSetConstantBuffers(0, 1, this->constantBuffer.getAddressOf());

	// Set buffers
	UINT offset = 0;
	this->pContext->IASetVertexBuffers(0, 1, vertexBuffer.getAddressOf(), vertexBuffer.getStridePtr(), &offset);
	this->pContext->IASetIndexBuffer(indexBuffer.get(), DXGI_FORMAT_R32_UINT, 0);

	this->pContext->DrawIndexed(indexBuffer.getBufferSize(), 0, 0);

	// Draw text
	spriteBatch->Begin();
	spriteFont->DrawString(
		spriteBatch.get(),
		L"Scintillating Mares",
		DirectX::XMFLOAT2(0.0f, 0.0f),
		DirectX::Colors::White,
		0.0f,
		DirectX::XMFLOAT2(0.0f, 0.0f),
		DirectX::XMFLOAT2(0.5f, 0.5f));
	spriteBatch->End();

	// ImGui
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	// Create a dummy test window
	ImGui::Begin("Test");
	ImGui::InputText("Input text test", str0, IM_ARRAYSIZE(str0));
	ImGui::End();
	// Assemble draw data
	ImGui::Render();
	// Render
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	// Don't call present() since we are hooked into the beginning of present.
	// Calling it creates infinite loop    *derp*
	// I just don't know what went wrooooooooooooooo...
	//this->pSwapchain->Present(1, NULL);
}

Graphics::~Graphics() {
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

bool Graphics::LinkDirectX(IDXGISwapChain* pSwapchain) {
	this->pSwapchain = pSwapchain;

	HRESULT hr = pSwapchain->GetDevice(__uuidof(ID3D11Device), (void**)&this->pDevice);
	if (FAILED(hr)) {
		Logger::Log(hr, "Failed to get D3D device");
		return false;
	}

	this->pDevice->GetImmediateContext(&this->pContext);
	this->pContext->OMGetRenderTargets(1, &this->pRenderTargetView, &this->depthStencilView);

	// If for some reason we fail to get a render target, create one.
	// This will probably never happen with a real game but maybe certain test environments... :P
	if (!pRenderTargetView)
	{
		Logger::Log("Failed to get render target, creating our own backup.");

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
	// Apparently this isn't universal. Works on some games
	pContext->RSGetViewports(&numViewports, this->pViewports);

	if (!numViewports || !this->pViewports[MAINVP].Width)
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
		this->pViewports[MAINVP].Width = (float)width;
		this->pViewports[MAINVP].Height = (float)height;
		this->pViewports[MAINVP].MinDepth = 0.0f;
		this->pViewports[MAINVP].MaxDepth = 1.0f;

		// Set viewport to context
		pContext->RSSetViewports(1, this->pViewports);
	}
	else
	{
		width = (float)this->pViewports[MAINVP].Width;
		height = (float)this->pViewports[MAINVP].Height;
	}

	return true;
}

bool Graphics::InitDirectX()
{
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

	rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
	HRESULT hr = this->pDevice->CreateRasterizerState(&rasterizerDesc, &this->pRasterizerState);

	if (FAILED(hr)) {
		Logger::Log(hr, "Failed to create rasterizer state.");
		return false;
	}

	std::filesystem::path fontLocation = getDllPath().parent_path() / "Resource" / "Fonts" / "celestial_redux_64.spritefont";
	spriteBatch = std::make_unique<DirectX::SpriteBatch>(this->pContext);
	spriteFont = std::make_unique<DirectX::SpriteFont>(this->pDevice, fontLocation.c_str());

	// Sampler state
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = this->pDevice->CreateSamplerState(&samplerDesc, &this->samplerState);
	if (FAILED(hr)) {
		Logger::Log(hr, "Failed to create sampler state.");
		return false;
	}

	return true;
}

bool Graphics::InitializeShaders() {

	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{
			"POSITION", // Name
			0,  // ID
			DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, // Format
			0,  // Input slot
			0,  // Offset
			D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, // Vertex or instance data
			0   // Instance data separate 
		},
		{
			"TEXCOORD", // Name
			0,  // ID
			DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, // Format
			0,  // Input slot
			D3D11_APPEND_ALIGNED_ELEMENT,  // Offset
			D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, // Vertex or instance data
			0   // Instance data separate 
		}
	};

	UINT numElements = ARRAYSIZE(layout);

	std::filesystem::path vertexShaderLocation = getDllPath().parent_path() / "vertexshader.cso";
	if (!vertexShader.Initialize(pDevice, vertexShaderLocation, layout, numElements)) return false;

	std::filesystem::path pixelShaderLocation = getDllPath().parent_path() / "pixelshader.cso";
	if (!pixelShader.Initialize(pDevice, pixelShaderLocation)) return false;

	return true;
}

bool Graphics::InitializeScene() {

	TexLocation texCoords = fm.getTextureLocation(0);

	printf("%f, %f | %f, %f\n", texCoords.x1, texCoords.y1, texCoords.x2, texCoords.y2);

	Vertex2 v[] = {
		 Vertex2( -0.5f, -0.5f, 0.0f, texCoords.x1,  texCoords.y2), // Bottom Left
		 Vertex2( -0.5f,  0.5f, 0.0f, texCoords.x1,  texCoords.y1), // Top Left
		 Vertex2(  0.5f, -0.5f, 0.0f, texCoords.x2,  texCoords.y2), // Bottom Right
		 Vertex2(  0.5f,  0.5f, 0.0f, texCoords.x2,  texCoords.y1), // Top Right
	};

	DWORD indices[] = {
		0, 1, 2, 3, 2, 1
	};

	// Vertex buffer
	HRESULT hr = this->vertexBuffer.initialize(this->pDevice, v, ARRAYSIZE(v));
	if (FAILED(hr)) {
		Logger::Log(hr, "Failed to create vertex buffer.");
		return false;
	}

	// Index buffer
	hr = this->indexBuffer.initialize(this->pDevice, indices, ARRAYSIZE(indices));
	if (FAILED(hr)) {
		Logger::Log(hr, "Failed to create index buffer.");
		return false;
	}

	// Texture
	hr = CoInitialize(NULL);
	if (FAILED(hr)) {
		Logger::Log(hr, "Failed to coinitialize.");
		return false;
	}

	std::filesystem::path textureLocation = getDllPath().parent_path() / "Resource" / "Textures" / "atlas.png";
	hr = DirectX::CreateWICTextureFromFile(this->pDevice, textureLocation.c_str(), nullptr, &itemsTexture);
	if (FAILED(hr)) {
		Logger::Log(hr, "Failed to create texture atlas.png.");
		return false;
	}
	
	// Constant buffer
	hr = this->constantBuffer.Initialize(pDevice, pContext);
	if (FAILED(hr)) {
		Logger::Log(hr, "Failed to create constant buffer.");
		return false;
	}

	return true;
}

bool Graphics::InitializeObjects() {
	fm = FileManager();
	if (!fm.Initialize()) return false;

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

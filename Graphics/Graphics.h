#pragma once

#include <d3d11.h>
#include <DirectXTK/SpriteBatch.h>
#include <DirectXTK/SpriteFont.h>
#include <DirectXTK/WICTextureLoader.h>
#include "Vertex.h"
#include "../Logger.h"
#include "../LoE/FileManager.h"
#include "Shader.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "DirectXTK.lib")


struct HandleData {
	DWORD pid;
	HWND hWnd;
};

class Graphics {
public:
	bool Initialize(IDXGISwapChain* pSwapchain);
	void RenderFrame();
	~Graphics();

private:
	bool LinkDirectX(IDXGISwapChain* pSwapchain);
	bool InitDirectX();
	bool InitializeShaders();
	bool InitializeScene();
	bool InitializeObjects();

	ID3D11Device* pDevice = nullptr;
	IDXGISwapChain* pSwapchain = nullptr;
	ID3D11DeviceContext* pContext = nullptr;
	ID3D11RenderTargetView* pRenderTargetView = nullptr;
	D3D11_VIEWPORT pViewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE]{ 0 };

	VertexBuffer<Vertex2> vertexBuffer;
	IndexBuffer indexBuffer;
	ConstantBuffer<CB_VS_vertexshader> constantBuffer;

	ID3D11RasterizerState* pRasterizerState;

	ID3D11DepthStencilView* depthStencilView;
	ID3D11Texture2D* depthStencilBuffer;

	ID3D11SamplerState* samplerState;
	ID3D11ShaderResourceView* itemsTexture;

	std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
	std::unique_ptr<DirectX::SpriteFont> spriteFont;

	VertexShader vertexShader;
	PixelShader pixelShader;

	float width, height, lastWidth, lastHeight;

	FileManager fm;
	char str0[128] = "Hello, world!";

	static HWND FindMainWindow(DWORD dwPID);
	static BOOL CALLBACK EnumWindowsCallback(HWND hWnd, LPARAM lParam);
};


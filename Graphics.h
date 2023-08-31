#pragma once

#include <d3d11.h>
#include "Vertex.h"
#include "Logger.h"
#include "Shader.h"
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "DirectXTK.lib")

struct HandleData {
	DWORD pid;
	HWND hWnd;
};

class Graphics
{
public:
	bool Initialize(IDXGISwapChain* pSwapchain);
	void RenderFrame();

private:
	bool LinkDirectX(IDXGISwapChain* pSwapchain);
	bool InitializeShaders();
	bool InitializeScene();

	ID3D11Device* pDevice = nullptr;
	IDXGISwapChain* pSwapchain = nullptr;
	ID3D11DeviceContext* pContext = nullptr;
	ID3D11RenderTargetView* pRenderTargetView = nullptr;

	ID3D11Buffer* pVertexBuffer;

	static HWND FindMainWindow(DWORD dwPID);
	static BOOL CALLBACK EnumWindowsCallback(HWND hWnd, LPARAM lParam);

	VertexShader vertexShader;
	PixelShader pixelShader;

	float width, height;
};


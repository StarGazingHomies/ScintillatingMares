#pragma once

#include "Logger.h"
#include "Path.h"
#pragma comment(lib, "D3DCompiler.lib")
#include <d3d11.h>
#include <d3dcompiler.h>

class VertexShader {
public:
	bool Initialize(ID3D11Device* device, std::wstring shaderPath, D3D11_INPUT_ELEMENT_DESC* desc, UINT numElements);
	ID3D11VertexShader* getShader() const;
	ID3D10Blob* getBuffer() const;
	ID3D11InputLayout* getInputLayout() const;
private:
	ID3D11InputLayout* pInputLayout = nullptr;
	ID3D11VertexShader* shader = nullptr;
	ID3D10Blob* shaderBuffer = nullptr;
};

class PixelShader {
public:
	bool Initialize(ID3D11Device* device, std::filesystem::path shaderPath, D3D11_INPUT_ELEMENT_DESC* desc, UINT numElements);
	ID3D11PixelShader* getShader() const;
	ID3D10Blob* getBuffer() const;
private:
	ID3D11PixelShader* shader = nullptr;
	ID3D10Blob* shaderBuffer = nullptr;
};



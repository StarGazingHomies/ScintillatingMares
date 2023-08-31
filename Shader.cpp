#include "pch.h"
#include "Shader.h"

bool VertexShader::Initialize(ID3D11Device* device, std::wstring shaderPath, D3D11_INPUT_ELEMENT_DESC* layout, UINT numElements) {
	
	// Load file
	HRESULT hr = D3DReadFileToBlob(shaderPath.c_str(), &this->shaderBuffer);
	if (FAILED(hr)) {
		std::wstring errMsg = L"Failed to load shader at ";
		errMsg += shaderPath;
		Logger::Log(hr, errMsg);
		return false;
	}

	// Create shader
	hr = device->CreateVertexShader(
		this->shaderBuffer->GetBufferPointer(), 
		this->shaderBuffer->GetBufferSize(),
		NULL,
		&this->shader
	);

	if (FAILED(hr)) {
		std::wstring errMsg = L"Failed to compile vertex shader at ";
		errMsg += shaderPath;
		Logger::Log(hr, errMsg);
		return false;
	}
	return true;

	// Create input layout
	hr = device->CreateInputLayout(
		layout,
		numElements,
		this->shaderBuffer->GetBufferPointer(),
		this->shaderBuffer->GetBufferSize(),
		&this->pInputLayout
	);

	if (FAILED(hr)) {
		Logger::Log(hr, "Failed to create input layout");
		return false;
	}
}

ID3D11VertexShader* VertexShader::getShader() const {
	return this->shader;
}

ID3D10Blob* VertexShader::getBuffer() const {
	return this->shaderBuffer;
}

ID3D11InputLayout* VertexShader::getInputLayout() const {
	return this->pInputLayout;
}

bool PixelShader::Initialize(ID3D11Device* device, std::wstring shaderPath, D3D11_INPUT_ELEMENT_DESC* desc, UINT numElements) {
	// Load file
	HRESULT hr = D3DReadFileToBlob(shaderPath.c_str(), &this->shaderBuffer);
	if (FAILED(hr)) {
		std::wstring errMsg = L"Failed to load shader at ";
		errMsg += shaderPath;
		Logger::Log(hr, errMsg);
		return false;
	}

	// Create shader
	hr = device->CreatePixelShader(
		this->shaderBuffer->GetBufferPointer(),
		this->shaderBuffer->GetBufferSize(),
		NULL,
		&this->shader
	);

	if (FAILED(hr)) {
		std::wstring errMsg = L"Failed to compile pixel shader at ";
		errMsg += shaderPath;
		Logger::Log(hr, errMsg);
		return false;
	}
	return true;
}

ID3D11PixelShader* PixelShader::getShader() const {
	return this->shader;
}

ID3D10Blob* PixelShader::getBuffer() const {
	return this->shaderBuffer;
}

#include "../pch.h"
#include "Shader.h"

#define safe_release(p) if (p.Get() == nullptr) { p.Reset(); }

bool VertexShader::Initialize(ID3D11Device* device, std::wstring shaderPath, D3D11_INPUT_ELEMENT_DESC* layout, UINT numElements) {
	
	// Load file
	HRESULT hr = D3DReadFileToBlob(shaderPath.c_str(), this->shaderBuffer.GetAddressOf());
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
		this->shader.GetAddressOf());

	if (FAILED(hr)) {
		std::wstring errMsg = L"Failed to compile vertex shader at ";
		errMsg += shaderPath;
		Logger::Log(hr, errMsg);
		return false;
	}

	// Create input layout
	hr = device->CreateInputLayout(
		layout,
		numElements, 
		this->shaderBuffer->GetBufferPointer(),
		this->shaderBuffer->GetBufferSize(),
		this->pInputLayout.GetAddressOf()
	);

	if (FAILED(hr)) {
		Logger::Log(hr, "Failed to create input layout");
		return false;
	}

	return true;
}

ID3D11VertexShader* VertexShader::getShader() const {
	return this->shader.Get();
}

ID3D10Blob* VertexShader::getBuffer() const {
	return this->shaderBuffer.Get();
}

ID3D11InputLayout* VertexShader::getInputLayout() const {
	return this->pInputLayout.Get();
}

VertexShader::~VertexShader() {
	safe_release(this->shader);
	safe_release(this->pInputLayout);
}

bool PixelShader::Initialize(ID3D11Device* device, std::filesystem::path shaderPath) {
	// Load file
	HRESULT hr = D3DReadFileToBlob(shaderPath.c_str(), this->shaderBuffer.GetAddressOf());
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
		this->shader.GetAddressOf()
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
	return this->shader.Get();
}

ID3D10Blob* PixelShader::getBuffer() const {
	return this->shaderBuffer.Get();
}

PixelShader::~PixelShader() {
	safe_release(this->shader);
}
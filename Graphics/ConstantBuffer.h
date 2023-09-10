#pragma once

#include <d3d11.h>
#include "ConstantBufferTypes.h"
#include <wrl/client.h>
#include "../Logger.h"

template <class T>
class ConstantBuffer {
public:
	ConstantBuffer() {}

	~ConstantBuffer() {
		if (pBuffer.Get() == nullptr) {
			pBuffer.Reset();
		}
	}

	T data;

	ID3D11Buffer* get() const {
		return this->pBuffer.Get();
	}

	ID3D11Buffer* const* getAddressOf() const {
		return this->pBuffer.GetAddressOf();
	}

	HRESULT Initialize(ID3D11Device* device, ID3D11DeviceContext* context) {
		if (pBuffer.Get() == nullptr) {
			pBuffer.Reset();
		}

		this->pContext = context;

		D3D11_BUFFER_DESC constantBufferDesc;
		ZeroMemory(&constantBufferDesc, sizeof(constantBufferDesc));

		constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		constantBufferDesc.MiscFlags = 0;
		// Constant buffers are 16 byte aligned
		constantBufferDesc.ByteWidth = static_cast<UINT>(sizeof(T) + (16 - (sizeof(T) % 16)));

		HRESULT hr = device->CreateBuffer(&constantBufferDesc, 0, &pBuffer);
		return hr;
	}

	bool applyChanges() {
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hr = this->pContext->Map(pBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(hr)) {
			ErrorLogger::Log(hr, "Failed to map constant buffer.");
			return false;
		}
		CopyMemory(mappedResource.pData, &data, sizeof(T));
		this->pContext->Unmap(pBuffer.Get(), 0);
		return true;
	}

private:
	ConstantBuffer(const ConstantBuffer<T>& rhs);
	Microsoft::WRL::ComPtr<ID3D11Buffer> pBuffer;
	ID3D11DeviceContext* pContext = nullptr;
};
#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <vector>

class IndexBuffer {
public:
	IndexBuffer() {}

	~IndexBuffer() {
		if (pBuffer) { pBuffer->Release(); pBuffer = nullptr; }
	}

	ID3D11Buffer* get() const {
		return this->pBuffer;
	}

	ID3D11Buffer* const* getAddressOf() const {
		return &this->pBuffer;
	}

	UINT getBufferSize() const {
		return this->bufferSize;
	}

	HRESULT initialize(ID3D11Device* device, DWORD* data, UINT numIndices) {
		this->bufferSize = numIndices;

		D3D11_BUFFER_DESC indexBufferDesc;
		ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(DWORD) * numIndices;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA indexBufferData;
		ZeroMemory(&indexBufferData, sizeof(data));
		indexBufferData.pSysMem = data;

		HRESULT hr = device->CreateBuffer(&indexBufferDesc, &indexBufferData, &this->pBuffer);
		return hr;
	}

private:
	IndexBuffer(const IndexBuffer& rhs);
	ID3D11Buffer* pBuffer;
	UINT bufferSize = 0;
};
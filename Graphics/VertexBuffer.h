#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include "Vertex.h"

template <class T>
class VertexBuffer {
public:
	VertexBuffer() {};
	~VertexBuffer() {
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

	const UINT getStride() const {
		return *this->stride.get();
	}

	const UINT* getStridePtr() const {
		return this->stride.get();
	}

	HRESULT initialize(ID3D11Device* device, T* data, UINT numVertices) {
		this->bufferSize = numVertices;
		this->stride = std::make_unique<UINT>(sizeof(T));

		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = sizeof(T) * numVertices;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA vertexBufferData;
		ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
		vertexBufferData.pSysMem = data;

		HRESULT hr = device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &this->pBuffer);
		return hr;
	}

private:
	VertexBuffer(const VertexBuffer<T>& rhs);
	ID3D11Buffer* pBuffer;
	std::unique_ptr<UINT> stride;
	UINT bufferSize = 0;
};


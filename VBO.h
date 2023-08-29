#pragma once
#include <vector>
#include <d3d11.h>

// Wrappers, because it makes code look better!

class VBO {
	ID3D11Buffer* buffer;
	ID3D11InputLayout* layout;

	VBO(std::vector<float> vertices, ID3D11InputLayout* layout);

	void Bind();
};


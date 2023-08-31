#pragma once

#include <DirectXMath.h>

struct Vertex2D
{
	Vertex2D() {}
	Vertex2D(float x, float y) {
		pos = DirectX::XMFLOAT2(x, y);
	}

	DirectX::XMFLOAT2 pos = { 0.0f, 0.0f };
};


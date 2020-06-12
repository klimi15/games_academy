#pragma once

#include <DirectXMath.h>

namespace GamesAcademy
{
	using namespace DirectX;

	struct Sample03Vertex
	{
		XMFLOAT3		postion;
		XMFLOAT2		texCoord;
	};

	struct Sample03ConstantData
	{
		XMFLOAT4X4		mvp;
	};
}

#pragma once

#include <DirectXMath.h>

namespace GamesAcademy
{
	using namespace DirectX;

	struct Sample04VertexSceneConstantData
	{
		XMFLOAT4X4	viewProjection;
		XMFLOAT4X4	shadowViewProjection;
	};

	struct Sample04VertexModelConstantData
	{
		XMFLOAT4X4	world;
		XMFLOAT4X4	normal;
	};
}

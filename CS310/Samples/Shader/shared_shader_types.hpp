#ifndef SHARED_SHADER_TYPES_INCLUDE
#define SHARED_SHADER_TYPES_INCLUDE

#ifdef __cplusplus

#define SAMPLE_SHADER_BEGIN	namespace GamesAcademy {
#define SAMPLE_SHADER_END	}

#include <DirectXMath.h>

namespace GamesAcademy
{
	using namespace DirectX;

	using float2 = XMFLOAT2;
	using float3 = XMFLOAT3;
	using float4 = XMFLOAT4;

	using float3x4 = XMFLOAT3X3;
	using float4x4 = XMFLOAT4X4;
}

#else

#define SAMPLE_SHADER_BEGIN
#define SAMPLE_SHADER_END

#endif

SAMPLE_SHADER_BEGIN

struct VertexViewConstantData
{
	float4x4	viewProjection;
	float4x4	shadowViewProjection;
};

struct VertexModelConstantData
{
	float4x4	world;
#ifdef __cplusplus
	// CPU 4x4 has same size as GPU 3x3
	float4x4	normal;
#else
	// Use 3x3 on GPU to avoid warnings
	float3x3	normal;
#endif
};

struct PixelLightConstants
{
	float4		param1;
	float4		param2;
	float4		param3;
};

#ifndef __cplusplus

float3 getPixelLightCameraPosition( PixelLightConstants constants )
{
	return constants.param1.xyz;
}

float3 getPixelLightDirection( PixelLightConstants constants )
{
	return float3( constants.param1.w, constants.param2.xy );
}

float getPixelLightIntensity( PixelLightConstants constants )
{
	return constants.param2.z;
}

float3 getPixelLightColor( PixelLightConstants constants )
{
	return float3( constants.param2.w, constants.param3.xy );
}

float2 getPixelLightShadowTexelSize( PixelLightConstants constants )
{
	return constants.param3.zw;
}

#else

void fillPixelLightConstants( PixelLightConstants& target, float3 cameraPosition, float3 lightDirection, float lightIntensity, float3 lightColor, float2 shadowTexelSize )
{
	target.param1 = { cameraPosition.x,		cameraPosition.y,	cameraPosition.z,	lightDirection.x };
	target.param2 = { lightDirection.y,		lightDirection.z,	lightIntensity,		lightColor.x };
	target.param3 = { lightColor.y,			lightColor.z,		shadowTexelSize.x,	shadowTexelSize.y };
}

#endif

SAMPLE_SHADER_END

#endif


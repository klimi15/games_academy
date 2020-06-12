#ifndef PBR_SHADER_TYPES_INCLUDE
#define PBR_SHADER_TYPES_INCLUDE

#include "shared_shader_types.hpp"

#define PBR_MODE_DEFAULT		0
#define PBR_MODE_SHADOW			1

#define PBR_PS_MODE_CONSTANT	2

SAMPLE_SHADER_BEGIN

struct PbrPixelMaterialConstants
{
	float4		param1;
	float4		param2;
};

#ifndef __cplusplus

struct VertexInput
{
	float3		position		: POSITION0;
#if PBR_MODE != PBR_MODE_SHADOW
	float2		uv				: TEXCOORD0;
	float3		normal			: NORMAL0;
	float4		tangentFlip		: TANGENT0;
#endif
};

struct VertexToPixel
{
	float4		position		: SV_POSITION0;
#if PBR_MODE != PBR_MODE_SHADOW
	float2		uv				: TEXCOORD0;
	float3		normal			: NORMAL0;
	float4		tangentFlip		: TANGENT0;
	float3		worldPosition	: TEXCOORD1;
	float4		shadowPosition	: TEXCOORD2;
#endif
};

struct PixelOutput
{
	float4		color			: SV_TARGET0;
};

float3 getPbrPixelMaterialColor( PbrPixelMaterialConstants constants )
{
	return constants.param1.xyz;
}

float getPbrPixelMaterialAmbientOcclusion( PbrPixelMaterialConstants constants )
{
	return constants.param1.w;
}

float getPbrPixelMaterialMetallic( PbrPixelMaterialConstants constants )
{
	return constants.param2.x;
}

float getPbrPixelMaterialRoughness( PbrPixelMaterialConstants constants )
{
	return constants.param2.y;
}

#else

void fillPbrPixelMaterialConstants( PbrPixelMaterialConstants& target, float3 color, float ambientOcclusion, float metallic, float roughness )
{
	target.param1 = { color.x,	color.y,	color.z,	ambientOcclusion };
	target.param2 = { metallic, roughness,	0.0f,		0.0f };
}

#endif

SAMPLE_SHADER_END

#endif

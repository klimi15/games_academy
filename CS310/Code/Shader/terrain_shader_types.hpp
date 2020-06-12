#ifndef TERRAIN_SHADER_TYPES_INCLUDE
#define TERRAIN_SHADER_TYPES_INCLUDE

#include "shared_shader_types.hpp"

#define TERRAIN_MODE_DEFAULT	0
#define TERRAIN_MODE_SHADOW		1

SAMPLE_SHADER_BEGIN

struct TerrainHullConstants
{
	float4	params1;
};

#ifndef __cplusplus

struct VertexInputOutput
{
	float3		position		: POSITION0;
	float2		uv				: TEXCOORD0;
};

struct PatchConstantOutput
{
	float edges[ 4 ]			: SV_TessFactor;
	float inside[ 2 ]			: SV_InsideTessFactor;
};

struct HullOutput
{
	float3		position		: POSITION0;
	float2		uv				: TEXCOORD0;
#if TERRAIN_MODE != TERRAIN_MODE_SHADOW
	float3		color			: COLOR0;
#endif
};

struct DomainOutput
{
	float4		position		: SV_POSITION0;
#if TERRAIN_MODE != TERRAIN_MODE_SHADOW
	float2		uv				: TEXCOORD0;
	float3		normal			: NORMAL0;
	float3		binormal		: BINORMAL0;
	float3		tangent			: TANGENT0;
	float3		worldPosition	: TEXCOORD1;
	float4		shadowPosition	: TEXCOORD2;
	float3		color			: COLOR0;
#endif
};

struct PixelOutput
{
	float4		color			: SV_TARGET0;
};

float3 getTerrainHullCameraPosition( TerrainHullConstants constants )
{
	return constants.params1.xyz;
}

float getTerrainHullTessellationFactor( TerrainHullConstants constants )
{
	return constants.params1.w;
}

#else

void fillTerrainHullConstants( TerrainHullConstants* pTarget, float3 cameraPosition, float tessellationFactor )
{
	pTarget->params1 = XMFLOAT4( cameraPosition.x, cameraPosition.y, cameraPosition.z, tessellationFactor );
}

#endif

SAMPLE_SHADER_END

#endif


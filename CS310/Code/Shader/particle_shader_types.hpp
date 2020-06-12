#ifndef PARTICLE_SHADER_TYPES_INCLUDE
#define PARTICLE_SHADER_TYPES_INCLUDE

#include "shared_shader_types.hpp"

#define PARTICLE_MODE_DEFAULT		0
#define PARTICLE_MODE_SHADOW		1

SAMPLE_SHADER_BEGIN

struct ParticleSimulationState
{
	float		lifeTime;
};

struct ParticleRenderState
{
	float3		position;
	float4		color;
	float2		scale;
	float		rotation;
};

struct ParticleComputeConstantData
{
	float4		params1;
};

struct ParticleGeometryViewConstantData
{
	float4x4	viewTranspose;
	float4x4	viewProjection;
};

#ifndef __cplusplus

struct VertexOutput
{
	float4		position	: SV_POSITION0;
	float4		color		: COLOR0;
	float2		scale		: TEXCOORD0;
	float		rotation	: TEXCOORD1;
};

struct GeometryOutput
{
	float4		position	: SV_POSITION0;
	float2		uv			: TEXCOORD0;
	float4		color		: COLOR0;
};

struct PixelOutput
{
	float4		color		: SV_TARGET0;
};

float getParticleComputeCount( ParticleComputeConstantData constants )
{
	return constants.params1.x;
}

float getParticleComputeGameTime( ParticleComputeConstantData constants )
{
	return constants.params1.y;
}

float getParticleComputeDeltaTime( ParticleComputeConstantData constants )
{
	return constants.params1.z;
}

#else

void fillParticleComputeConstantData( ParticleComputeConstantData* pTarget, float particleCount, float gameTime, float deltaTime )
{
	pTarget->params1 = { particleCount, gameTime, deltaTime, 0.0f };
}

// setter

#endif

SAMPLE_SHADER_END

#endif



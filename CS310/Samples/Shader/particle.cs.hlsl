// PARTICLE_MODE=1

#include "particle_shader_types.hpp"

#include "shared.hlsli"

static const float GroupSize = 1024.0;

RWStructuredBuffer< ParticleSimulationState > b_simulation : register( u0 );
RWStructuredBuffer< ParticleRenderState > b_render : register( u1 );

cbuffer c_constants : register(b0)
{
	ParticleComputeConstantData c_particle;
};

[numthreads( 512, 1, 1 )]
void main( uint3 id : SV_DispatchThreadID )
{
	float groupCount	= getParticleComputeCount( c_particle ) / GroupSize;
	float gameTime		= getParticleComputeGameTime( c_particle );
	float groupId		= floor( id.x / GroupSize );
	float groupIdNorm	= groupId / groupCount;
	float elementId		= id.x % GroupSize;
	float elementIdNorm	= elementId / GroupSize;

	float startOffset	= random( float2( groupIdNorm, 0.4853 ) );
	float progress		= (startOffset + ((gameTime % 4.0) / 4.0)) % 1;
	float variation		= random( float2( elementIdNorm, 0.2587 ) ) * 2.0 - 1.0;
	
	float xAngle = random( float2( groupIdNorm, 0.7854 ) ) - 0.5; //(groupId / groupCount) * PI * 8.0;
	float yAngle = (groupId / groupCount) * PI * 2.0;
	float yzAngle = startOffset + elementIdNorm * PI * 2.0;

	float3 spherePosition = float3(
		sin( -yAngle ) * 2.0,
		sin( -xAngle ) * 2.0,
		cos( -yAngle ) * 2.0
	);

	float3x3 sphereMatrix = float3x3(
		cos( yAngle ),	0.0,	sin( yAngle ),
		0.0,			1.0,	0.0,
		-sin( yAngle ),	0.0,	cos( yAngle )
	);

	float3 ringPosition = float3(
		0.0,
		cos( yzAngle + gameTime ),
		sin( yzAngle + gameTime )
	);
	ringPosition *= progress;
	ringPosition = mul( ringPosition, sphereMatrix );

	float3 position;
	position = spherePosition + ringPosition;
	
	float4 color;
	color.r		= 1.0;
	color.g		= 1.0 - (variation / 5.0);
	color.b		= 0.0;
	color.a		= 1.0 - progress - abs(variation / 5.0);
	
	float scale = (elementIdNorm / 5.0) + (variation / 4.0);
	scale *= scale;

	//position.x	= sin( elementIdNorm * PI * 2.0 ) * 5.0;
	//position.y	= sin( (elementIdNorm * PI * 12.0) + gameTime ) + 5.0;
	//position.z	= cos( elementIdNorm * PI * 2.0 ) * 5.0;

	//position.x	= (elementIdNorm - 0.5) * 8.0;
	//position.y	= 5.0;
	//position.z	= (elementIdNorm - 0.5) * 32.0;

	ParticleRenderState result;
	result.position		= position;
	result.color		= color;
	result.scale		= float2( scale, scale );
	result.rotation		= 0.0;

	b_render[ id.x ] = result;
}
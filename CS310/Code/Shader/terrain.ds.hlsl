// TERRAIN_MODE=2

#include "terrain_shader_types.hpp"

cbuffer c_view_constants : register(b0)
{
	VertexViewConstantData c_view;
};

cbuffer c_model_constants : register(b1)
{
	VertexModelConstantData c_model;
};

Texture2D t_height			: register( t0 );
SamplerState s_clamp_linear	: register( s0 );

[domain( "quad" )]
DomainOutput main( PatchConstantOutput input, float2 domainLocation : SV_DomainLocation, const OutputPatch< HullOutput, 4 > patches )
{
	float3 interpolatedPosition	= lerp( lerp( patches[ 0 ].position, patches[ 1 ].position, domainLocation.x ), lerp( patches[ 2 ].position, patches[ 3 ].position, domainLocation.x ), domainLocation.y );
	float2 interpolatedTexCoord	= lerp( lerp( patches[ 0 ].uv, patches[ 1 ].uv, domainLocation.x ), lerp( patches[ 2 ].uv, patches[ 3 ].uv, domainLocation.x ), domainLocation.y );

	float height = t_height.SampleLevel( s_clamp_linear, interpolatedTexCoord, 0.0 ).r * 2625.0;
	interpolatedPosition.y = height;

	float4 position = float4(interpolatedPosition, 1.0);
	position = mul( position, c_model.world );
	position = mul( position, c_view.viewProjection );

#if TERRAIN_MODE != TERRAIN_MODE_SHADOW
	float2 texCoordOffset = float2( 1.0 / 4096.0, 1.0 / 4096.0 ) * 1.0;
	float heightL = t_height.SampleLevel( s_clamp_linear, interpolatedTexCoord + (texCoordOffset * float2( -1.0,  0.0 )), 0.0 ).r * 2625.0;
	float heightR = t_height.SampleLevel( s_clamp_linear, interpolatedTexCoord + (texCoordOffset * float2(  1.0,  0.0 )), 0.0 ).r * 2625.0;
	float heightT = t_height.SampleLevel( s_clamp_linear, interpolatedTexCoord + (texCoordOffset * float2(  0.0, -1.0 )), 0.0 ).r * 2625.0;
	float heightB = t_height.SampleLevel( s_clamp_linear, interpolatedTexCoord + (texCoordOffset * float2(  0.0,  1.0 )), 0.0 ).r * 2625.0;

	// alternative normal without tangent
	//float3 normal = normalize( float3(heightL - heightR, 2.0, heightT - heightB) );

	float3 tangent		= float3( 0.0, heightT - heightB, -2.0 );
	float3 binormal		= float3( 2.0, heightR - heightL,  0.0);
	float3 normal		= normalize( cross( binormal, tangent ) );
	normal				= mul( normal, c_model.normal );
	binormal			= mul( binormal, c_model.normal );
	tangent				= mul( tangent, c_model.normal );

	float4 shadowPosition = float4( interpolatedPosition, 1.0 );
	shadowPosition = mul( shadowPosition, c_model.world );
	shadowPosition = mul( shadowPosition, c_view.shadowViewProjection );

	float4 worldPosition = float4( interpolatedPosition, 1.0 );
	worldPosition = mul( worldPosition, c_model.world );
#endif

	DomainOutput output;
	output.position			= position;

#if TERRAIN_MODE != TERRAIN_MODE_SHADOW
	output.uv				= interpolatedTexCoord;
	output.normal			= normal;
	output.binormal			= binormal;
	output.tangent			= tangent;

	output.shadowPosition	= shadowPosition;
	output.worldPosition	= worldPosition.xyz;

	output.color			= lerp( lerp( patches[ 0 ].color, patches[ 1 ].color, domainLocation.x ), lerp( patches[ 2 ].color, patches[ 3 ].color, domainLocation.x ), domainLocation.y );
#endif

	return output;
}

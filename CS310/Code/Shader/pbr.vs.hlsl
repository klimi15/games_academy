// PBR_MODE=2

#include "pbr_shader_types.hpp"

cbuffer c_view_constants : register( b0 )
{
	VertexViewConstantData c_view;
};

cbuffer c_model_constants : register( b1 )
{
	VertexModelConstantData c_model;
};

VertexToPixel main( VertexInput input )
{
	float4 position = float4( input.position, 1.0 );
	position = mul( position, c_model.world );
	position = mul( position, c_view.viewProjection );

#if PBR_MODE != PBR_MODE_SHADOW
	float4 shadowPosition = float4( input.position, 1.0 );
	shadowPosition = mul( shadowPosition, c_model.world );
	shadowPosition = mul( shadowPosition, c_view.shadowViewProjection );

	float4 worldPosition = float4( input.position, 1.0 );
	worldPosition = mul( worldPosition, c_model.world );

	float3 normal = input.normal;
	normal = mul( normal, c_model.normal );

	float3 tangent = input.tangentFlip.xyz;
	tangent = mul( tangent, c_model.normal );
#endif

	VertexToPixel output;
	output.position			= position;

#if PBR_MODE != PBR_MODE_SHADOW
	output.uv				= input.uv;
	output.normal			= normal;
	output.tangentFlip		= float4( tangent, input.tangentFlip.w );

	output.shadowPosition	= shadowPosition;
	output.worldPosition	= worldPosition.xyz;
#endif

	return output;
}

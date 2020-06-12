// PARTICLE_MODE=1

#include "particle_shader_types.hpp"

StructuredBuffer< ParticleRenderState > b_input : register( t0 );

cbuffer c_model_constants : register( b1 )
{
	VertexModelConstantData c_model;
};

VertexOutput main( uint vertexId : SV_VertexID )
{
	ParticleRenderState input = b_input.Load( vertexId );

	float4 position = float4( input.position, 1.0);
	position = mul( position, c_model.world );

	VertexOutput output;
	output.position = position;
	output.color	= input.color;
	output.scale	= input.scale;
	output.rotation	= input.rotation;

	return output;
}
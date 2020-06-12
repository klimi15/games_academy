// PARTICLE_MODE=1

#include "particle_shader_types.hpp"

cbuffer c_view_constants : register(b0)
{
	ParticleGeometryViewConstantData c_view;
};

[maxvertexcount( 4 )]
void main( point VertexOutput inputs[ 1 ], inout TriangleStream< GeometryOutput > outputs )
{
	VertexOutput input = inputs[ 0 ];

	float3 a	= c_view.viewTranspose[ 2 ].xyz;
	//float s		= sin( input.rotation );
	//float c		= cos( input.rotation );
	//
	//float xx = a.x * a.x;
	//float yy = a.y * a.y;
	//float zz = a.z * a.z;
	//float xy = a.x * a.y;
	//float xz = a.x * a.z;
	//float yz = a.y * a.z;
	//
	//float4x4 billboardRotation = float4x4(
	//	xx + c * (1 - xx),		xy - c * xy + s * a.z,	xz - c * xz - s * a.y,	0.0,
	//	xy - c * xy - s * a.z,	yy + c * (1 - yy),		yz - c * yz + s * a.x,	0.0,
	//	xz - c * xz + s * a.y,	yz + c * yz - s * a.x,	zz + c * (1 - zz),		0.0,
	//	0.0,					0.0,					0.0,					1.0
	//);

	float4 right	= float4( normalize( c_view.viewTranspose[ 0 ].xyz ), 0) * input.scale.x;
	float4 up		= float4( normalize( c_view.viewTranspose[ 1 ].xyz ), 0) * input.scale.y;

	GeometryOutput output;
	output.color	= input.color;

	output.position	= input.position + ((-right) + (-up));
	output.position = mul( output.position, c_view.viewProjection );
	output.uv		= float2( 0.0, 0.0 );
	outputs.Append( output );

	output.position	= input.position + ((-right) + up);
	output.position = mul( output.position, c_view.viewProjection );
	output.uv		= float2( 0.0, 1.0 );
	outputs.Append( output );

	output.position	= input.position + (right + (-up));
	output.position = mul( output.position, c_view.viewProjection );
	output.uv		= float2( 1.0, 0.0 );
	outputs.Append( output );

	output.position = input.position + (right + up);
	output.position = mul( output.position, c_view.viewProjection );
	output.uv		= float2( 1.0, 1.0 );
	outputs.Append( output );

	outputs.RestartStrip();
}
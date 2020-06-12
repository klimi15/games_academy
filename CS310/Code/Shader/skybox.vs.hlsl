struct VertexInput
{
	float3	position	: POSITION0;
};

struct VertexToPixel
{
	float4 position		: SV_POSITION0;
	float3 uvw			: TEXCOORD0;
};

cbuffer c_scene_constants : register(b0)
{
	float4x4	viewProjection;
	float4x4	shadowViewProjection;
};

cbuffer c_model_constants : register(b1)
{
	float4x4	world;
	float4x4	normal;
};

VertexToPixel main( VertexInput input )
{
	float4 position = float4( input.position, 1.0 );
	position = mul( position, world );
	position = mul( position, viewProjection );

	VertexToPixel output;
	output.position	= position;
	output.uvw		= input.position;

	return output;
}
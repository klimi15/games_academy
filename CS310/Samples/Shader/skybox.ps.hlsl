struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float3 uvw			: TEXCOORD0;
};

TextureCube t_environmentMap : register( t0 );

SamplerState s_wrap_anisotropic : register( s0 );

float4 main( VertexToPixel input ) : SV_TARGET
{
	float3 environmentSample = t_environmentMap.Sample( s_wrap_anisotropic, input.uvw ).rgb;
	return float4( environmentSample, 1.0f );
}
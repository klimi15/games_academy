// PBR_MODE=3

#include "pbr_shader_types.hpp"

#include "shared.hlsli"

#if PBR_MODE == PBR_MODE_SHADOW
void main( VertexToPixel input )
{
}
#else
Texture2D t_shadowMap			: register( t0 );
#if PBR_MODE == PBR_MODE_DEFAULT
Texture2D t_albedo				: register( t1 );
Texture2D t_normal				: register( t2 );
Texture2D t_ambient_occlusion	: register( t3 );
Texture2D t_roughness			: register( t4 );
Texture2D t_metalness			: register( t5 );
#endif

SamplerState s_wrap_anisotropic	: register( s0 );
SamplerState s_clamp_point		: register( s1 );

cbuffer c_lightData : register( b0 )
{
	PixelLightConstants			c_light;
};

#if PBR_MODE == PBR_PS_MODE_CONSTANT
cbuffer c_materialData : register( b1 )
{
	PbrPixelMaterialConstants	c_material;
};
#endif

PixelOutput main( VertexToPixel input )
{
	PixelOutput output;
#if PBR_MODE == PBR_MODE_DEFAULT
	float3 normal		= input.normal;
	float3 tangent		= input.tangentFlip.xyz;
	float3 binormal		= cross( normal, tangent ) * input.tangentFlip.w;

	float3 normalSample		= sampleNormalMap( t_normal, s_wrap_anisotropic, input.uv );
	normal					= normalize( normalSample.x * tangent + normalSample.y * binormal + normalSample.z * normal );
#else
	float3 normal			= normalize( input.normal );
#endif

#if PBR_MODE == PBR_MODE_DEFAULT
	float3 albedo			= t_albedo.Sample( s_wrap_anisotropic, input.uv ).rgb;
	float ambient_occlusion	= t_ambient_occlusion.Sample( s_wrap_anisotropic, input.uv ).r;
	float metallic			= t_metalness.Sample( s_wrap_anisotropic, input.uv ).r;
	float roughness			= t_roughness.Sample( s_wrap_anisotropic, input.uv ).r;
#else
	float3 albedo			= getPbrPixelMaterialColor( c_material );
	float ambient_occlusion	= getPbrPixelMaterialAmbientOcclusion( c_material );
	float metallic			= getPbrPixelMaterialMetallic( c_material );
	float roughness			= getPbrPixelMaterialRoughness( c_material );
#endif

	float shadow			= calculateShadow( t_shadowMap, s_clamp_point, input.shadowPosition, getPixelLightDirection( c_light ), normal, getPixelLightShadowTexelSize( c_light ) );
	float3 radiance			= getPixelLightColor( c_light ) * getPixelLightIntensity( c_light ) * shadow;
	float3 aomr				= float3( ambient_occlusion, metallic, roughness );
	float3 color			= calculatePBR( albedo, aomr, normal, getPixelLightDirection( c_light ), getPixelLightCameraPosition( c_light ), input.worldPosition, radiance );

	output.color = float4( color, 1.0 );

	return output;
}
#endif

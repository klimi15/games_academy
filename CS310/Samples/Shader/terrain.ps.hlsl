// TERRAIN_MODE=2

#include "terrain_shader_types.hpp"

#include "shared.hlsli"

#if TERRAIN_MODE == TERRAIN_MODE_SHADOW
void main( DomainOutput input )
{
}
#else
Texture2D t_shadowMap			: register( t0 );
Texture2D t_blendMap			: register( t2 );

Texture2D t_albedo1				: register( t3 );
Texture2D t_normal1				: register( t4 );
Texture2D t_ambient_occlusion1	: register( t5 );
Texture2D t_roughness1			: register( t6 );

Texture2D t_albedo2				: register( t7 );
Texture2D t_normal2				: register( t8 );
Texture2D t_ambient_occlusion2	: register( t9 );
Texture2D t_roughness2			: register( t10 );

Texture2D t_albedo3				: register( t11 );
Texture2D t_normal3				: register( t12 );
Texture2D t_ambient_occlusion3	: register( t13 );
Texture2D t_roughness3			: register( t14 );

SamplerState s_wrap_anisotropic	: register( s0 );
SamplerState s_clamp_point		: register( s1 );
SamplerState s_clamp_linear		: register( s2 );

cbuffer c_lightData : register( b0 )
{
	PixelLightConstants			c_light;
};

PixelOutput main( DomainOutput input )
{
	float3 normal		= normalize( input.normal );
	float3 tangent		= normalize( input.tangent );
	float3 binormal		= normalize( input.binormal );
	float3x3 tbn		= float3x3( tangent, binormal, normal );

	float textureScale = 8.0;
	float2 uvx = input.worldPosition.zy / textureScale;
	float2 uvy = input.worldPosition.xz / textureScale;
	float2 uvz = input.worldPosition.xy / textureScale;

	if( normal.x < 0 )
	{
		uvx.x = -uvx.x;
	}
	if( normal.y < 0 )
	{
		uvy.x = -uvy.x;
	}
	if( normal.z >= 0 )
	{
		uvz.x = -uvz.x;
	}

	uvx.y += 0.5;
	uvz.x += 0.5;

	float3 blendWeights = abs( normal );
	blendWeights = blendWeights / (blendWeights.x + blendWeights.y + blendWeights.z);

	float shadow				= calculateShadow( t_shadowMap, s_clamp_point, input.shadowPosition, getPixelLightDirection( c_light ), normal, getPixelLightShadowTexelSize( c_light ) );

	float3 albedo1				= sampleTriplanarRGB( t_albedo1, s_wrap_anisotropic, uvx, uvy, uvz, blendWeights );
	float3 albedo2				= sampleTriplanarRGB( t_albedo2, s_wrap_anisotropic, uvx, uvy, uvz, blendWeights );
	float3 albedo3				= sampleTriplanarRGB( t_albedo3, s_wrap_anisotropic, uvx, uvy, uvz, blendWeights );

	float3 normal1				= sampleTriplanarNormal( t_normal1, s_wrap_anisotropic, uvx, uvy, uvz, blendWeights, tbn );
	float3 normal2				= sampleTriplanarNormal( t_normal2, s_wrap_anisotropic, uvx, uvy, uvz, blendWeights, tbn );
	float3 normal3				= sampleTriplanarNormal( t_normal3, s_wrap_anisotropic, uvx, uvy, uvz, blendWeights, tbn );

	float ambient_occlusion1	= sampleTriplanarR( t_ambient_occlusion1, s_wrap_anisotropic, uvx, uvy, uvz, blendWeights );
	float ambient_occlusion2	= sampleTriplanarR( t_ambient_occlusion2, s_wrap_anisotropic, uvx, uvy, uvz, blendWeights );
	float ambient_occlusion3	= sampleTriplanarR( t_ambient_occlusion3, s_wrap_anisotropic, uvx, uvy, uvz, blendWeights );

	float roughness1			= sampleTriplanarR( t_roughness1, s_wrap_anisotropic, uvx, uvy, uvz, blendWeights );
	float roughness2			= sampleTriplanarR( t_roughness2, s_wrap_anisotropic, uvx, uvy, uvz, blendWeights );
	float roughness3			= sampleTriplanarR( t_roughness3, s_wrap_anisotropic, uvx, uvy, uvz, blendWeights );

	float3 radiance				= getPixelLightColor( c_light ) * getPixelLightIntensity( c_light ) * shadow;
	float3 color1				= calculatePBR( albedo1, float3( ambient_occlusion1, 0.0, roughness1 ), normal1, getPixelLightDirection( c_light ), getPixelLightCameraPosition( c_light ), input.worldPosition, radiance );
	float3 color2				= calculatePBR( albedo2, float3( ambient_occlusion2, 0.0, roughness2 ), normal1, getPixelLightDirection( c_light ), getPixelLightCameraPosition( c_light ), input.worldPosition, radiance );
	float3 color3				= calculatePBR( albedo3, float3( ambient_occlusion3, 0.0, roughness3 ), normal1, getPixelLightDirection( c_light ), getPixelLightCameraPosition( c_light ), input.worldPosition, radiance );

	float3 blend = t_blendMap.SampleLevel( s_clamp_linear, input.uv, 0.0 ).rgb;
	blend = blend / (blend.x + blend.y + blend.z);
	float3 color = color1 * blend.x + color2 * blend.y + color3 * blend.z;

	PixelOutput output;
	output.color = float4( color, 1.0 );

	return output;
}
#endif

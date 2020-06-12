static const float PI = 3.14159265;

float3 sampleNormalMap( Texture2D normalMap, SamplerState normalSampler, float2 uv )
{
	return normalMap.Sample( normalSampler, uv ).xyz * 2.0 - 1.0;
}

float sampleTriplanarR( Texture2D map, SamplerState mapSampler, float2 uvx, float2 uvy, float2 uvz, float3 blendWeights )
{
	float sampleX = map.Sample( mapSampler, uvx ).r;
	float sampleY = map.Sample( mapSampler, uvy ).r;
	float sampleZ = map.Sample( mapSampler, uvz ).r;

	return sampleX * blendWeights.x + sampleY * blendWeights.y + sampleZ * blendWeights.z;
}

float3 sampleTriplanarRGB( Texture2D map, SamplerState mapSampler, float2 uvx, float2 uvy, float2 uvz, float3 blendWeights )
{
	float3 sampleX = map.Sample( mapSampler, uvx ).rgb;
	float3 sampleY = map.Sample( mapSampler, uvy ).rgb;
	float3 sampleZ = map.Sample( mapSampler, uvz ).rgb;

	return sampleX * blendWeights.x + sampleY * blendWeights.y + sampleZ * blendWeights.z;
}

float3 sampleTriplanarNormal( Texture2D map, SamplerState mapSampler, float2 uvx, float2 uvy, float2 uvz, float3 blendWeights, float3x3 tbn )
{
	float3 sampleX = map.Sample( mapSampler, uvx ).rgb;
	float3 sampleY = map.Sample( mapSampler, uvy ).rgb;
	float3 sampleZ = map.Sample( mapSampler, uvz ).rgb;

	float3 normal = (sampleX * blendWeights.x + sampleY * blendWeights.y + sampleZ * blendWeights.z) * 2.0 - 1.0;
	return normalize( mul( normal, tbn ) );
}

float calculateLight( float3 lightDirection, float3 normal )
{
	return max( dot( lightDirection, normal ), 0.0 );
}

float calculateShadow( Texture2D shadowMap, SamplerState shadowSampler, float4 shadowPosition, float3 lightDirection, float3 normal, float2 shadowTexelSize )
{
	float3 normalizedShadowPosition = shadowPosition.xyz / shadowPosition.w;
	float2 projectedShadowTexCoord = float2(
		0.5 + (normalizedShadowPosition.x * 0.5f),
		0.5 - (normalizedShadowPosition.y * 0.5f)
	);

	float shadowBias		= clamp( 0.005 * tan( acos( calculateLight( lightDirection, normal ) ) ), 0.0, 0.01 );
	float geometryDepth		= normalizedShadowPosition.z - shadowBias;

	float shadow = 1.0;
	if( saturate( projectedShadowTexCoord.x ) == projectedShadowTexCoord.x &&
		saturate( projectedShadowTexCoord.y ) == projectedShadowTexCoord.y &&
		saturate( geometryDepth ) == geometryDepth )
	{
		float shadowSampleCC	= shadowMap.Sample( shadowSampler, projectedShadowTexCoord ).r;
		float shadowSamplePC	= shadowMap.Sample( shadowSampler, projectedShadowTexCoord + (shadowTexelSize * float2(  1.0f,  0.0 )) ).r;
		float shadowSampleNC	= shadowMap.Sample( shadowSampler, projectedShadowTexCoord + (shadowTexelSize * float2( -1.0f,  0.0 )) ).r;
		float shadowSampleCP	= shadowMap.Sample( shadowSampler, projectedShadowTexCoord + (shadowTexelSize * float2(  0.0f,  1.0 )) ).r;
		float shadowSampleCN	= shadowMap.Sample( shadowSampler, projectedShadowTexCoord + (shadowTexelSize * float2(  0.0f, -1.0 )) ).r;

		shadow =
			(geometryDepth < shadowSampleCC ? 1.0 : 0.0) +
			(geometryDepth < shadowSamplePC ? 1.0 : 0.0) +
			(geometryDepth < shadowSampleNC ? 1.0 : 0.0) +
			(geometryDepth < shadowSampleCP ? 1.0 : 0.0) +
			(geometryDepth < shadowSampleCN ? 1.0 : 0.0);

		shadow /= 5.0;
	}

	return shadow;
}

float3 fresnelSchlick( float cosTheta, float3 F0 )
{
    return F0 + (1.0 - F0) * pow( 1.0 - cosTheta, 5.0 );
}

float distributionGGX( float3 N, float3 H, float roughness )
{
    float a      = roughness * roughness;
    float a2     = a * a;
    float NdotH  = max( dot( N, H ), 0.0 );
    float NdotH2 = NdotH * NdotH;

    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

float geometrySchlickGGX( float NdotV, float roughness )
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}

float geometrySmith( float3 N, float3 V, float3 L, float roughness )
{
    float NdotV = max( dot( N, V ), 0.0 );
    float NdotL = max( dot( N, L ), 0.0 );
    float ggx2  = geometrySchlickGGX( NdotV, roughness );
    float ggx1  = geometrySchlickGGX( NdotL, roughness );

    return ggx1 * ggx2;
}

float3 calculatePBR( float3 albedo, float3 aomr, float3 normal, float3 lightDirection, float3 cameraPosition, float3 worldPosition, float3 radiance )
{
	float ambient_occlusion	= aomr.x;
	float metallic			= aomr.y;
	float roughness			= aomr.z;

	float3 N				= normal;
	float3 L				= normalize( lightDirection );
	float3 V				= normalize( cameraPosition - worldPosition );
	float3 H				= normalize( V + L );

	float3 F0				= float3( 0.04, 0.04, 0.04 );
	F0						= lerp( F0, albedo, metallic.rrr );
	float3 F				= fresnelSchlick( max( dot( H, V ), 0.0 ), F0 );

	float NDF				= distributionGGX( N, H, roughness );
	float G					= geometrySmith( N, V, L, roughness );

	float3 numerator		= NDF * G * F;
	float denominator		= 4.0 * max( dot( N, V ), 0.0 ) * max( dot( N, L ), 0.0 );
	float3 specular			= numerator / max( denominator, 0.001 );

	float3 kS = F;
	float3 kD = float3( 1.0, 1.0, 1.0 ) - kS;
	kD *= 1.0 - metallic;

	float NdotL = max( dot( N, L ), 0.0 );

	float3 Lo = (kD * albedo / PI + specular) * radiance * NdotL;
	float3 ambient = float3( 0.06, 0.06, 0.06 ) * albedo * ambient_occlusion;

	//return pow( normal * 0.5 + 0.5, 1.0 / 2.2 );
	//return albedo;
	//return ambient_occlusion;
	//return metallic;
	//return roughness;
	//return ambient;
	//return specular;
	//return radiance;
	//return NdotL;
	//return Lo;
	return ambient + Lo;
}

float random( float2 uv )
{
    return frac( sin( dot( uv, float2( 12.9898, 78.233 ) ) ) * 43758.5453 );
}

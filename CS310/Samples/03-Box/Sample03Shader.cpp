#include "Sample03Shader.hpp"

namespace GamesAcademy
{
	static const char VertexShader[] = R"V0G0N(
		struct VertexInput
		{
			float3		position	: POSITION0;
			float2		uv			: TEXCOORD0;
		};

		struct VertexToPixel
		{
			float4		position	: SV_POSITION0;
			float2		uv			: TEXCOORD0;
		};

		cbuffer constants : register(b0)
		{
			float4x4	mvp;
		};

		VertexToPixel main( VertexInput input )
		{
			float4 position = float4( input.position, 1.0 );
			position = mul( position, mvp );

			VertexToPixel output;
			output.position		= position;
			output.uv			= input.uv;

			return output;
		}
	)V0G0N";

	MemoryBlock Sample03Shader::getVertexShader()
	{
		return { VertexShader, sizeof( VertexShader ) };
	}

	static const char PixelShader[] = R"V0G0N(
		struct VertexToPixel
		{
			float4		position	: SV_POSITION0;
			float2		uv			: TEXCOORD0;
		};

		struct PixelOutput
		{
			float4		color		: SV_TARGET0;
		};

		Texture2D texture0 : register( t0 );
		SamplerState sampler0 : register( s0 );

		PixelOutput main( VertexToPixel input )
		{
			PixelOutput output;
			output.color = texture0.Sample( sampler0, input.uv );

			return output;
		}
	)V0G0N";

	MemoryBlock Sample03Shader::getPixelShader()
	{
		return { PixelShader, sizeof( PixelShader ) };
	}
}

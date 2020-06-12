#include "Sample02Shader.hpp"

namespace GamesAcademy
{
	static const char VertexShader[] = R"V0G0N(
		struct VertexInput
		{
			float2		position	: POSITION0;
			float3		color		: COLOR0;
		};

		struct VertexToPixel
		{
			float4		position	: SV_POSITION0;
			float3		color		: COLOR0;
		};

		VertexToPixel main( VertexInput input )
		{
			VertexToPixel output;
			output.position		= float4( input.position, 0.0, 1.0 );
			output.color		= input.color;

			return output;
		}
	)V0G0N";

	MemoryBlock Sample02Shader::getVertexShader()
	{
		return { VertexShader, sizeof( VertexShader ) };
	}

	static const char PixelShader[] = R"V0G0N(
		struct VertexToPixel
		{
			float4		position	: SV_POSITION0;
			float3		color		: COLOR0;
		};

		struct PixelOutput
		{
			float4		color		: SV_TARGET0;
		};

		PixelOutput main( VertexToPixel input )
		{
			PixelOutput output;
			output.color		= float4( input.color, 1.0 );

			return output;
		}
	)V0G0N";

	MemoryBlock Sample02Shader::getPixelShader()
	{
		return { PixelShader, sizeof( PixelShader ) };
	}
}

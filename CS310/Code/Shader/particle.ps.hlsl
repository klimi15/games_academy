
#include "particle_shader_types.hpp"

Texture2D t_color				: register( t0 );

SamplerState s_clamp_linear		: register( s2 );

PixelOutput main( GeometryOutput input )
{
	PixelOutput output;
	output.color = input.color * t_color.Sample( s_clamp_linear, input.uv );

	return output;
}
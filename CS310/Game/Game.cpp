#include "Game.h"

namespace GA
{
	static const char* WindowTitle = u8"CS310 Game 🎮";
	static const int WindowWidth = 1280;
	static const int WindowHeight = 720;

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

		VertexToPixel main( VertexInput input )
		{
			VertexToPixel output;
			output.position		= float4( input.position, 1.0 );
			output.uv			= input.uv;

			return output;
		}
	)V0G0N";

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

		PixelOutput main( VertexToPixel input )
		{
			PixelOutput output;
			output.color		= float4( input.uv, 1.0, 1.0 );

			return output;
		}
	)V0G0N";

	int Game::run()
	{
		if( !m_graphics.create( WindowTitle, WindowWidth, WindowHeight ) ||
			!m_input.create( m_graphics ) )
		{
			return 1;
		}

		m_vertexShader	= m_graphics.createVertexShader( { VertexShader, sizeof( VertexShader ) }, GraphicsVertexFormat::Position3_Uv2 );
		m_pixelShader	= m_graphics.createPixelShader( { PixelShader, sizeof( PixelShader ) } );

		m_plane.createPlane( m_graphics );

		m_running = true;
		while( m_running )
		{
			update();
			render();
		}

		m_plane.destroy();
		m_input.destroy();
		m_graphics.destroy();

		return 0;
	}

	void Game::update()
	{
		m_graphics.update();
		m_input.update();

		m_running &= m_graphics.isOpen() && !m_input.isKeyboardKeyDown( InputKeyboardKey::Escape );
	}

	void Game::render()
	{
		const float bgColor[ 4u ] = { 0.0f, 0.0f, 0.5f, 1.0f };
		ID3D11DeviceContext* pContext = m_graphics.beginFrame( bgColor );

		m_graphics.applyShader( m_vertexShader );
		m_graphics.applyShader( m_pixelShader );
		m_plane.render( m_graphics, pContext );

		m_graphics.endFrame();
	}
}

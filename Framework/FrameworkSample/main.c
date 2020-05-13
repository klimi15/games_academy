#include "framework.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <windows.h>

static ga_graphics_color BackgroundColor;
static ga_graphics_color ButtonColor;
static ga_graphics_color ButtonBorderColor;
static ga_graphics_color ButtonIconColor;
static ga_graphics_color MouseColor;

static const float ButtonWidth						= 256.0f;
static const float ButtonHeight						= 64.0f;
static const float ButtonIconSize					= 48.0f;
static const float ButtonAnimationSize				= 8.0f;
static const float ButtonAnimationSpeed				= 5.0f;
static const float MouseSize						= 12.0f;

bool doButton( ga_graphics_t* pGraphics, ga_input_t* pInput, float centerX, float centerY, float width, float height, float* pState, double gameTime )
{
	const float borderSize	= 2.0f;
	const float halfWidth	= width * 0.5f;
	const float halfHeight	= height * 0.5f;
	const float left		= centerX - halfWidth;
	const float right		= centerX + halfWidth;
	const float top			= centerY - halfHeight;
	const float bottom		= centerY + halfHeight;

	const float mouseX		= ga_input_get_mouse_position_x( pInput );
	const float mouseY		= ga_input_get_mouse_position_y( pInput );
	const bool isHovered	= (mouseX >= left && mouseY >= top && mouseX <= right && mouseY <= bottom);

	float state = *pState;
	if( !isHovered )
	{
		state = 0.0f;
	}
	else
	{
		const float sinus = sinf( (float)gameTime * ButtonAnimationSpeed );
		state = ((sinus + 1.0f) / 2.0f) * ButtonAnimationSize;
	}

	ga_graphics_draw_rect(
		pGraphics,
		left - state,
		top - state,
		width + (state * 2.0f),
		height + (state * 2.0f),
		ButtonBorderColor
	);

	ga_graphics_draw_rect(
		pGraphics,
		left + borderSize - state,
		top + borderSize - state,
		width - (borderSize * 2.0f) + (state * 2.0f),
		height - (borderSize * 2.0f) + (state * 2.0f),
		ButtonColor
	);

	*pState = state;
	return isHovered && ga_input_was_mouse_button_pressed( pInput, ga_input_mouse_button_left );
}

int __stdcall WinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd )
{
	BackgroundColor		= ga_graphics_color_rgb( 0x01u, 0x65u, 0xfcu );
	ButtonColor			= ga_graphics_color_rgb( 0x05u, 0x04u, 0xaau );
	ButtonBorderColor	= ga_graphics_color_rgb( 0xa2u, 0xcfu, 0xfeu );
	ButtonIconColor		= ga_graphics_color_rgb( 0xa2u, 0xcfu, 0xfeu );
	MouseColor			= ga_graphics_color_rgb( 0x13u, 0xeau, 0xc9u );

	ga_timer_t* pTimer = ga_timer_create();
	if( !pTimer )
	{
		return 1u;
	}

	ga_graphics_t* pGraphics = ga_graphics_create( 1280, 720, u8"Framework Sample 🚀" );
	if( !pGraphics )
	{
		return 1u;
	}

	ga_input_t* pInput = ga_input_create( pGraphics );
	if( !pInput )
	{
		ga_graphics_destroy( pGraphics );
		return 1u;
	}

	ga_sound_t* pSound = ga_sound_create();
	if( !pInput )
	{
		ga_input_destroy( pInput );
		ga_graphics_destroy( pGraphics );
		return 1u;
	}

	ga_sound_file_t* pSoundFile = ga_sound_load_file( pSound, "C:\\Windows\\Media\\Alarm02.wav" );

	bool running = true;
	float playStopButtonState = 0.0f;
	float exitButtonState = 0.0f;
	ga_sound_voice_t* pVoice = NULL;
	while( running && ga_graphics_is_window_open( pGraphics ) )
	{
		const float gameTime = (float)ga_timer_get( pTimer );

		ga_input_update( pInput );

		ga_graphics_begin_frame( pGraphics );

		ga_graphics_clear( pGraphics, BackgroundColor );

		const float halfWindowWidth = ga_graphics_get_back_buffer_width( pGraphics ) * 0.5f;
		const float halfWindowHeight = ga_graphics_get_back_buffer_height( pGraphics ) * 0.5f;

		const float playStopButtonCenterY = halfWindowHeight - (ButtonHeight * 0.75f);
		if( !ga_sound_is_playing( pSound, pVoice ) )
		{
			// play button
			if( doButton( pGraphics, pInput, halfWindowWidth, playStopButtonCenterY, ButtonWidth, ButtonHeight, &playStopButtonState, gameTime ) )
			{
				pVoice = ga_sound_play( pSound, pSoundFile );
			}

			ga_graphics_draw_triangle(
				pGraphics,
				halfWindowWidth - (ButtonIconSize * 0.5f),
				playStopButtonCenterY - (ButtonIconSize * 0.5f),
				halfWindowWidth + (ButtonIconSize * 0.5f),
				playStopButtonCenterY,
				halfWindowWidth - (ButtonIconSize * 0.5f),
				playStopButtonCenterY + (ButtonIconSize * 0.5f),
				ButtonIconColor
			);
		}
		else
		{
			// stop button
			if( doButton( pGraphics, pInput, halfWindowWidth, playStopButtonCenterY, ButtonWidth, ButtonHeight, &playStopButtonState, gameTime ) )
			{
				ga_sound_stop( pSound, pVoice );
				pVoice = NULL;
			}

			ga_graphics_draw_rect(
				pGraphics,
				halfWindowWidth - (ButtonIconSize * 0.5f),
				playStopButtonCenterY - (ButtonIconSize * 0.5f),
				ButtonIconSize,
				ButtonIconSize,
				ButtonIconColor
			);
		}

		// exit button
		const float exitButtonCenterY = halfWindowHeight + (ButtonHeight * 0.75f);
		if( doButton( pGraphics, pInput, halfWindowWidth, exitButtonCenterY, ButtonWidth, ButtonHeight, &exitButtonState, gameTime ) )
		{
			running = false;
		}

		ga_graphics_draw_rect(
			pGraphics,
			halfWindowWidth - (ButtonIconSize * 0.5f),
			exitButtonCenterY - (ButtonIconSize * 0.5f),
			ButtonIconSize,
			ButtonIconSize,
			ButtonIconColor
		);

		// mouse cursor
		const float mouseTime1 = -gameTime + (float)M_PI * 2.0f * 0.333f;
		const float mouseTime2 = -gameTime;
		const float mouseTime3 = -gameTime + (float)M_PI * 2.0f * 0.666f;
		const float mouseX		= ga_input_get_mouse_position_x( pInput );
		const float mouseY		= ga_input_get_mouse_position_y( pInput );
		const float mouseX1		= mouseX + (sinf( mouseTime1 ) * MouseSize);
		const float mouseY1		= mouseY + (cosf( mouseTime1 ) * MouseSize);
		const float mouseX2		= mouseX + (sinf( mouseTime2 ) * MouseSize);
		const float mouseY2		= mouseY + (cosf( mouseTime2 ) * MouseSize);
		const float mouseX3		= mouseX + (sinf( mouseTime3 ) * MouseSize);
		const float mouseY3		= mouseY + (cosf( mouseTime3 ) * MouseSize);

		ga_graphics_draw_triangle(
			pGraphics,
			mouseX1, mouseY1,
			mouseX2, mouseY2,
			mouseX3, mouseY3,
			MouseColor
		);

		// end frame
		ga_graphics_end_frame( pGraphics );
	}

	ga_sound_free_file( pSound, pSoundFile );

	ga_sound_destroy( pSound );
	ga_input_destroy( pInput );
	ga_graphics_destroy( pGraphics );
	ga_timer_destroy( pTimer );
	return 0u;
}
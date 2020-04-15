#include "framework.h"

#include "graphics.hpp"
#include "input.hpp"
#include "sound.hpp"
#include "timer.hpp"

GamesAcademy::Graphics* getGraphicsClass( ga_graphics_t* pGraphics )
{
	return (GamesAcademy::Graphics*)pGraphics;
}

GamesAcademy::Input* getInputClass( ga_input_t* pInput )
{
	return (GamesAcademy::Input*)pInput;
}

GamesAcademy::Sound* getSoundClass( ga_sound_t* pSound )
{
	return (GamesAcademy::Sound*)pSound;
}

ga_timer_t* GA_CDECL ga_timer_create()
{
	GamesAcademy::Timer* pCppTimer = new GamesAcademy::Timer();
	pCppTimer->create();
	return (ga_timer_t*)pCppTimer;
}

void GA_CDECL ga_timer_destroy( ga_timer_t* pTimer )
{
	GamesAcademy::Timer* pCppTimer = (GamesAcademy::Timer*)pTimer;
	delete pCppTimer;
}

double GA_CDECL ga_timer_get( ga_timer_t* pTimer )
{
	GamesAcademy::Timer* pCppTimer = (GamesAcademy::Timer*)pTimer;
	return pCppTimer->get();
}

ga_graphics_t* GA_CDECL ga_graphics_create( int windowWidth, int windowHeight, const char* pWindowTitleUtf8 )
{
	GamesAcademy::Graphics* pCppGraphics = new  GamesAcademy::Graphics();
	if( !pCppGraphics->create( windowWidth, windowHeight, pWindowTitleUtf8 ) )
	{
		delete pCppGraphics;
		return nullptr;
	}

	return (ga_graphics_t*)pCppGraphics;
}

void GA_CDECL ga_graphics_destroy( ga_graphics_t* pGraphics )
{
	GamesAcademy::Graphics* pCppGraphics = getGraphicsClass( pGraphics );
	pCppGraphics->destroy();
	delete pCppGraphics;
}

bool GA_CDECL ga_graphics_is_window_open( ga_graphics_t* pGraphics )
{
	return getGraphicsClass( pGraphics )->isWindowOpen();
}

float GA_CDECL ga_graphics_get_back_buffer_width( ga_graphics_t* pGraphics )
{
	return getGraphicsClass( pGraphics )->getBackBufferWidth();
}

float GA_CDECL ga_graphics_get_back_buffer_height( ga_graphics_t* pGraphics )
{
	return getGraphicsClass( pGraphics )->getBackBufferHeight();
}

void GA_CDECL ga_graphics_begin_frame( ga_graphics_t* pGraphics )
{
	getGraphicsClass( pGraphics )->beginFrame();
}

void GA_CDECL ga_graphics_clear( ga_graphics_t* pGraphics, ga_graphics_color color )
{
	getGraphicsClass( pGraphics )->clear( color );
}

void GA_CDECL ga_graphics_draw_rect( ga_graphics_t* pGraphics, float x, float y, float width, float height, ga_graphics_color color )
{
	getGraphicsClass( pGraphics )->drawRect( x, y, width, height, color );
}

void GA_CDECL ga_graphics_draw_triangle( ga_graphics_t* pGraphics, float x1, float y1, float x2, float y2, float x3, float y3, ga_graphics_color color )
{
	getGraphicsClass( pGraphics )->drawTriangle( x1, y1, x2, y2, x3, y3, color );
}

void GA_CDECL ga_graphics_end_frame( ga_graphics_t* pGraphics )
{
	getGraphicsClass( pGraphics )->endFrame();
}

ga_graphics_color GA_CDECL ga_graphics_color_rgb( uint8_t r, uint8_t g, uint8_t b )
{
	return ga_graphics_color_rgba( r, g, b, 0xffu );
}

ga_graphics_color GA_CDECL ga_graphics_color_rgb_float( float r, float g, float b )
{
	return ga_graphics_color_rgba_float( r, g, b, 1.0f );
}

ga_graphics_color GA_CDECL ga_graphics_color_rgba( uint8_t r, uint8_t g, uint8_t b, uint8_t a )
{
	return (uint32_t( r ) << 0u) | (uint32_t( g ) << 8u) | (uint32_t( b ) << 16u) | (uint32_t( a ) << 24u);
}

ga_graphics_color GA_CDECL ga_graphics_color_rgba_float( float r, float g, float b, float a )
{
	return ga_graphics_color_rgba( uint8_t( r * 255.0f ), uint8_t( g * 255.0f ), uint8_t( b * 255.0f ), uint8_t( a * 255.0f ) );
}

ga_input_t* GA_CDECL ga_input_create( ga_graphics_t* pGraphics )
{
	GamesAcademy::Input* pCppInput = new GamesAcademy::Input();
	if( !pCppInput->create( getGraphicsClass( pGraphics ) ) )
	{
		delete pCppInput;
		return nullptr;
	}

	return (ga_input_t*)pCppInput;
}

void GA_CDECL ga_input_destroy( ga_input_t* pInput )
{
	GamesAcademy::Input* pCppInput = getInputClass( pInput );
	pCppInput->destroy();
	delete pCppInput;
}

void GA_CDECL ga_input_update( ga_input_t* pInput )
{
	getInputClass( pInput )->update();
}

float GA_CDECL ga_input_get_mouse_delta_x( ga_input_t* pInput )
{
	return getInputClass( pInput )->getMouseDeltaX();
}

float GA_CDECL ga_input_get_mouse_delta_y( ga_input_t* pInput )
{
	return getInputClass( pInput )->getMouseDeltaY();
}

float GA_CDECL ga_input_get_mouse_position_x( ga_input_t* pInput )
{
	return getInputClass( pInput )->getMousePositionX();
}

float GA_CDECL ga_input_get_mouse_position_y( ga_input_t* pInput )
{
	return getInputClass( pInput )->getMousePositionY();
}

bool GA_CDECL ga_input_is_mouse_button_down( ga_input_t* pInput, ga_input_mouse_button button )
{
	return getInputClass( pInput )->isMouseButtonDown( button );
}

bool GA_CDECL ga_input_is_mouse_button_up( ga_input_t* pInput, ga_input_mouse_button button )
{
	return getInputClass( pInput )->isMouseButtonUp( button );
}

bool GA_CDECL ga_input_was_mouse_button_pressed( ga_input_t* pInput, ga_input_mouse_button button )
{
	return getInputClass( pInput )->wasMouseButtonPressed( button );
}

bool GA_CDECL ga_input_was_mouse_button_released( ga_input_t* pInput, ga_input_mouse_button button )
{
	return getInputClass( pInput )->wasMouseButtonReleased( button );
}

ga_sound_t* GA_CDECL ga_sound_create()
{
	GamesAcademy::Sound* pCppSound = new GamesAcademy::Sound();
	if( !pCppSound->create() )
	{
		delete pCppSound;
		return nullptr;
	}

	return (ga_sound_t*)pCppSound;
}

void GA_CDECL ga_sound_destroy( ga_sound_t* pSound )
{
	GamesAcademy::Sound* pCppSound = getSoundClass( pSound );
	pCppSound->destroy();
	delete pCppSound;
}

ga_sound_file_t* GA_CDECL ga_sound_load_file( ga_sound_t* pSound, const char* pFilename )
{
	return (ga_sound_file_t*)getSoundClass( pSound )->loadFile( pFilename );
}

void GA_CDECL ga_sound_free_file( ga_sound_t* pSound, ga_sound_file_t* pFile )
{
	getSoundClass( pSound )->freeFile( pFile );
}

void GA_CDECL ga_sound_play( ga_sound_t* pSound, ga_sound_file_t* pFile )
{
	getSoundClass( pSound )->play( pFile );
}

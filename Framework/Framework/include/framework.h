#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
#	define GA_CDECL __stdcall
#else
#	define GA_CDECL
#endif


#ifdef __cplusplus
extern "C"
{
#endif

//////////////////////////////////////////////////////////////////////////
// Time

struct ga_timer;
typedef struct ga_timer ga_timer_t;

ga_timer_t*			GA_CDECL ga_timer_create();
void				GA_CDECL ga_timer_destroy( ga_timer_t* pTimer );

double				GA_CDECL ga_timer_get( ga_timer_t* pTimer ); // returns the elapsed seconds since the timer was created


//////////////////////////////////////////////////////////////////////////
// Graphics

struct ga_graphics;
typedef struct ga_graphics ga_graphics_t;

typedef int32_t ga_graphics_color;

ga_graphics_t*		GA_CDECL ga_graphics_create( int windowWidth, int windowHeight, const char* pWindowTitleUtf8 );
void				GA_CDECL ga_graphics_destroy( ga_graphics_t* pGraphics );

bool				GA_CDECL ga_graphics_is_window_open( ga_graphics_t* pGraphics );

float				GA_CDECL ga_graphics_get_back_buffer_width( ga_graphics_t* pGraphics );
float				GA_CDECL ga_graphics_get_back_buffer_height( ga_graphics_t* pGraphics );

void				GA_CDECL ga_graphics_begin_frame( ga_graphics_t* pGraphics );

void				GA_CDECL ga_graphics_clear( ga_graphics_t* pGraphics, ga_graphics_color color );
void				GA_CDECL ga_graphics_draw_rect( ga_graphics_t* pGraphics, float x, float y, float width, float height, ga_graphics_color color );
void				GA_CDECL ga_graphics_draw_triangle( ga_graphics_t* pGraphics, float x1, float y1, float x2, float y2, float x3, float y3, ga_graphics_color color );

void				GA_CDECL ga_graphics_end_frame( ga_graphics_t* pGraphics );

ga_graphics_color	GA_CDECL ga_graphics_color_rgb( uint8_t r, uint8_t g, uint8_t b );
ga_graphics_color	GA_CDECL ga_graphics_color_rgb_float( float r, float g, float b );
ga_graphics_color	GA_CDECL ga_graphics_color_rgba( uint8_t r, uint8_t g, uint8_t b, uint8_t a );
ga_graphics_color	GA_CDECL ga_graphics_color_rgba_float( float r, float g, float b, float a );


//////////////////////////////////////////////////////////////////////////
// Input

struct ga_input;
typedef struct ga_input ga_input_t;

enum ga_input_mouse_button
{
	ga_input_mouse_button_left,
	ga_input_mouse_button_right,
	ga_input_mouse_button_middle
};

ga_input_t*			GA_CDECL ga_input_create( ga_graphics_t* pGraphics );
void				GA_CDECL ga_input_destroy( ga_input_t* pInput );

void				GA_CDECL ga_input_update( ga_input_t* pInput );

float				GA_CDECL ga_input_get_mouse_delta_x( ga_input_t* pInput );
float				GA_CDECL ga_input_get_mouse_delta_y( ga_input_t* pInput );
float				GA_CDECL ga_input_get_mouse_position_x( ga_input_t* pInput );
float				GA_CDECL ga_input_get_mouse_position_y( ga_input_t* pInput );

bool				GA_CDECL ga_input_is_mouse_button_down( ga_input_t* pInput, ga_input_mouse_button button );
bool				GA_CDECL ga_input_is_mouse_button_up( ga_input_t* pInput, ga_input_mouse_button button );
bool				GA_CDECL ga_input_was_mouse_button_pressed( ga_input_t* pInput, ga_input_mouse_button button );
bool				GA_CDECL ga_input_was_mouse_button_released( ga_input_t* pInput, ga_input_mouse_button button );


//////////////////////////////////////////////////////////////////////////
// Sound

struct ga_sound;
typedef struct ga_sound ga_sound_t;

struct ga_sound_file;
typedef struct ga_sound_file ga_sound_file_t;

ga_sound_t*			GA_CDECL ga_sound_create();
void				GA_CDECL ga_sound_destroy( ga_sound_t* pSound );

ga_sound_file_t*	GA_CDECL ga_sound_load_file( ga_sound_t* pSound, const char* pFilename );
void				GA_CDECL ga_sound_free_file( ga_sound_t* pSound, ga_sound_file_t* pFile );

void				GA_CDECL ga_sound_play( ga_sound_t* pSound, ga_sound_file_t* pFile );


#ifdef __cplusplus
}
#endif

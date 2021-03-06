#include "framework/graphics.hpp"
#include "framework/timer.hpp"

#include "font.h"

#include <windows.h>
#include <crtdbg.h>
#include <math.h>

namespace GamesAcademy
{
	int		run();
}

int __stdcall WinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd )
{
	return GamesAcademy::run();
}

int GamesAcademy::run()
{
	Graphics graphics;
	if( !graphics.create( 1280u, 720u, "Font" ) )
	{
		return 1;
	}

	Font font;
	if( !font.create( graphics, "C:\\Windows\\Fonts\\arial.ttf", 64 ) )
	{
		return 1;
	}

	Timer timer;
	timer.create();

	while( graphics.isWindowOpen() )
	{
		graphics.beginFrame();
		graphics.clear( 0x556699ffu );

		const float size = float( fabs( sin( timer.get() ) ) * 32.0f ) + 16.0f;
		graphics.drawText( 0.0f, 0.0f, size, font, "Hello World!", 0u );

		graphics.endFrame();
	}

	font.destroy();
	graphics.destroy();

	_CrtDumpMemoryLeaks();
	return 0;
}

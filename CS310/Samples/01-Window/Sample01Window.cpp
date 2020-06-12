#include "Sample01Window.hpp"

#include <d3d11.h>
#include <math.h>

namespace GamesAcademy
{
	static const char* WindowTitle = u8"CS310 01-Window 🍩";
	static const int WindowWidth = 1280;
	static const int WindowHeight = 720;

	int Sample01Window::run()
	{
		if( !m_window.create( WindowTitle, WindowWidth, WindowHeight ) ||
			!m_device.create( m_window, false ) )
		{
			m_device.destroy();
			m_window.destroy();
			return 1;
		}

		while( m_window.isOpen() )
		{
			update();
			render();
		}

		m_device.destroy();
		m_window.destroy();

		return 0;
	}

	void Sample01Window::update()
	{
		m_window.update();
		m_device.update();

		//m_backgroundColor[ 0u ] = sinf( time );
		//m_backgroundColor[ 1u ] = cosf( time );
		//m_backgroundColor[ 2u ] = 0.5f;
		//m_backgroundColor[ 3u ] = 1.0f;
	}

	void Sample01Window::render()
	{
		m_device.beginFrame( m_backgroundColor );
		m_device.endFrame();
	}
}

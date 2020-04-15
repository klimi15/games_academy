#pragma once

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

namespace GamesAcademy
{
	class Graphics;

	class Input
	{
	public:

		bool					create( Graphics* pGraphics );
		void					destroy();

		void					update();

		float					getMouseDeltaX() const;
		float					getMouseDeltaY() const;
		float					getMousePositionX() const;
		float					getMousePositionY() const;

		bool					isMouseButtonDown( int button ) const;
		bool					isMouseButtonUp( int button ) const;
		bool					wasMouseButtonPressed( int button ) const;
		bool					wasMouseButtonReleased( int button ) const;

	private:

		Graphics*				m_pGraphics;

		IDirectInputA*			m_pInputDevice;
		IDirectInputDeviceA*	m_pMouse;

		POINT					m_mousePosition;
		DIMOUSESTATE2			m_lastMouseState;
		DIMOUSESTATE2			m_currentMouseState;
	};
}
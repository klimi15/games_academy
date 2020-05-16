#include "input.hpp"

#include "graphics.hpp"

namespace GamesAcademy
{
	bool Input::create( Graphics* pGraphics )
	{
		m_pGraphics = pGraphics;

		const HINSTANCE hInstance = GetModuleHandle( nullptr );

		HRESULT result = DirectInput8Create( hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pInputDevice, nullptr );
		if( FAILED( result ) || m_pInputDevice == nullptr )
		{
			destroy();
			return false;
		}

		result = m_pInputDevice->CreateDevice( GUID_SysMouse, &m_pMouse, nullptr );
		if( SUCCEEDED( result ) && m_pMouse != nullptr )
		{
			result = m_pMouse->SetDataFormat( &c_dfDIMouse2 );
			if( SUCCEEDED( result ) )
			{
				result = m_pMouse->SetCooperativeLevel( m_pGraphics->getWindowHandle(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE );
				if( SUCCEEDED( result ) )
				{
					return true;
				}
			}
		}

		return false;
	}

	void Input::destroy()
	{
		if( m_pMouse != nullptr )
		{
			m_pMouse->Unacquire();

			m_pMouse->Release();
			m_pMouse = nullptr;
		}

		if( m_pInputDevice != nullptr )
		{
			m_pInputDevice->Release();
			m_pInputDevice = nullptr;
		}

		m_pGraphics = nullptr;
	}

	void Input::update()
	{
		m_lastMouseState = m_currentMouseState;

		const HRESULT result = m_pMouse->GetDeviceState( sizeof( m_currentMouseState ), &m_currentMouseState );
		if( FAILED( result ) &&
			(result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED) )
		{
			m_pMouse->Acquire();
		}

		GetCursorPos( &m_mousePosition );
		ScreenToClient( m_pGraphics->getWindowHandle(), &m_mousePosition );

		if( GetAsyncKeyState( VK_ESCAPE ) & 1 )
		{
			m_pGraphics->closeWindow();
		}
	}

	float Input::getMouseDeltaX() const
	{
		return float( m_currentMouseState.lX );
	}

	float Input::getMouseDeltaY() const
	{
		return float( m_currentMouseState.lY );
	}

	float Input::getMousePositionX() const
	{
		return float( m_mousePosition.x );
	}

	float Input::getMousePositionY() const
	{
		return float( m_mousePosition.y );
	}

	bool Input::isMouseButtonDown( int button ) const
	{
		return m_currentMouseState.rgbButtons[ button ] != 0u;
	}

	bool Input::isMouseButtonUp( int button ) const
	{
		return m_currentMouseState.rgbButtons[ button ] == 0u;
	}

	bool Input::wasMouseButtonPressed( int button ) const
	{
		return m_lastMouseState.rgbButtons[ button ] == 0u && m_currentMouseState.rgbButtons[ button ] != 0u;
	}

	bool Input::wasMouseButtonReleased( int button ) const
	{
		return m_lastMouseState.rgbButtons[ button ] != 0u && m_currentMouseState.rgbButtons[ button ] == 0u;
	}
}
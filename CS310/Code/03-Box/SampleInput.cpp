#include "SampleInput.hpp"

#include "../01-Window/SampleWindow.hpp"

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

namespace GamesAcademy
{
	static const uint8 s_keyboardMapping[] =
	{
		DIK_ESCAPE,
		DIK_SPACE,
		DIK_LSHIFT,
		DIK_F1,
		DIK_F2,
		DIK_F3,
		DIK_F4,
		DIK_F5,
		DIK_F6,
		DIK_F7,
		DIK_F8,
		DIK_F9,
		DIK_F10,
		DIK_F11,
		DIK_F12,
		DIK_W,
		DIK_A,
		DIK_S,
		DIK_D,
		DIK_LEFT,
		DIK_RIGHT,
		DIK_UP,
		DIK_DOWN
	};

	static_assert( ARRAY_COUNT( s_keyboardMapping ) == (uint8)SampleInputKeyboardKey::Count, "" );

	struct SampleInputState
	{
		POINT			mousePosition;
		DIMOUSESTATE2	lastMouseState;
		DIMOUSESTATE2	currentMouseState;

		uint8			lastKeyboardState[ 256u ];
		uint8			currentKeyboardState[ 256u ];
	};

	SampleInput::SampleInput()
	{
	}

	SampleInput::~SampleInput()
	{
		destroy();
	}

	bool SampleInput::create( SampleWindow& window )
	{
		m_pWindow	= &window;
		m_pState	= new SampleInputState();

		const HINSTANCE hInstance = GetModuleHandle( nullptr );

		HRESULT result = DirectInput8Create( hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pInputDevice, nullptr );
		if( FAILED( result ) || m_pInputDevice == nullptr )
		{
			destroy();
			return false;
		}

		bool mouseOk = false;
		result = m_pInputDevice->CreateDevice( GUID_SysMouse, &m_pMouse, nullptr );
		if( SUCCEEDED( result ) )
		{
			result = m_pMouse->SetDataFormat( &c_dfDIMouse2 );
			if( SUCCEEDED( result ) )
			{
				result = m_pMouse->SetCooperativeLevel( window.getHandle(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE );
				if( SUCCEEDED( result ) )
				{
					mouseOk = true;
				}
			}
		}

		bool keybaordOk = false;
		result = m_pInputDevice->CreateDevice( GUID_SysKeyboard, &m_pKeyboard, nullptr );
		if( SUCCEEDED( result ) )
		{
			if( SUCCEEDED( m_pKeyboard->SetDataFormat( &c_dfDIKeyboard ) ) )
			{
				result = m_pKeyboard->SetCooperativeLevel( window.getHandle(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE );
				if( SUCCEEDED( result ) )
				{
					keybaordOk = true;
				}
			}
		}

		if( !mouseOk || !keybaordOk )
		{
			destroy();
			return false;
		}

		return true;
	}

	void SampleInput::destroy()
	{
		if( m_pState != nullptr )
		{
			delete m_pState;
			m_pState = nullptr;
		}

		if( m_pKeyboard != nullptr )
		{
			m_pKeyboard->Unacquire();

			m_pKeyboard->Release();
			m_pKeyboard = nullptr;
		}

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

		m_pWindow = nullptr;
	}

	void SampleInput::update()
	{
		m_pState->lastMouseState = m_pState->currentMouseState;

		for( size_t i = 0u; i < ARRAY_COUNT( m_pState->currentKeyboardState ); ++i )
		{
			m_pState->lastKeyboardState[ i ] = m_pState->currentKeyboardState[ i ];
		}

		HRESULT result = m_pMouse->GetDeviceState( sizeof( m_pState->currentMouseState ), &m_pState->currentMouseState );
		if( FAILED( result ) &&
			(result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED) )
		{
			m_pMouse->Acquire();
		}

		result = m_pKeyboard->GetDeviceState( sizeof( m_pState->currentKeyboardState ), m_pState->currentKeyboardState );
		if( FAILED( result ) &&
			(result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED) )
		{
			m_pKeyboard->Acquire();
		}

		GetCursorPos( &m_pState->mousePosition );
	}

	float SampleInput::getMouseDeltaX() const
	{
		return float( m_pState->currentMouseState.lX );
	}

	float SampleInput::getMouseDeltaY() const
	{
		return float( m_pState->currentMouseState.lY );
	}

	float SampleInput::getMousePositionX() const
	{
		return float( m_pState->mousePosition.x );
	}

	float SampleInput::getMousePositionY() const
	{
		return float( m_pState->mousePosition.y );
	}

	bool SampleInput::isMouseButtonDown( SampleInputMouseButton button ) const
	{
		return m_pState->currentMouseState.rgbButtons[ (uint8)button ] != 0u;
	}

	bool SampleInput::isMouseButtonUp( SampleInputMouseButton button ) const
	{
		return m_pState->currentMouseState.rgbButtons[ (uint8)button ] == 0u;
	}

	bool SampleInput::wasMouseButtonPressed( SampleInputMouseButton button ) const
	{
		return m_pState->lastMouseState.rgbButtons[ (uint8)button ] == 0u && m_pState->currentMouseState.rgbButtons[ (uint8)button ] != 0u;
	}

	bool SampleInput::wasMouseButtonReleased( SampleInputMouseButton button ) const
	{
		return m_pState->lastMouseState.rgbButtons[ (uint8)button ] != 0u && m_pState->currentMouseState.rgbButtons[ (uint8)button ] == 0u;
	}

	bool SampleInput::isKeyboardKeyDown( SampleInputKeyboardKey key ) const
	{
		return (m_pState->currentKeyboardState[ s_keyboardMapping[ (uint8)key ] ] & 0x80u) != 0u;
	}

	bool SampleInput::isKeyboardKeyUp( SampleInputKeyboardKey key ) const
	{
		return (m_pState->currentKeyboardState[ s_keyboardMapping[ (uint8)key ] ] & 0x80u) == 0u;
	}

	bool SampleInput::wasKeyboardKeyPressed( SampleInputKeyboardKey key ) const
	{
		return (m_pState->currentKeyboardState[ s_keyboardMapping[ (uint8)key ] ] & 0x80u) != 0u && (m_pState->lastKeyboardState[ s_keyboardMapping[ (uint8)key ] ] & 0x80u) == 0u;
	}

	bool SampleInput::wasKeyboardKeyReleased( SampleInputKeyboardKey key ) const
	{
		return (m_pState->currentKeyboardState[ s_keyboardMapping[ (uint8)key ] ] & 0x80u) == 0u && (m_pState->lastKeyboardState[ s_keyboardMapping[ (uint8)key ] ] & 0x80u) != 0u;
	}
}

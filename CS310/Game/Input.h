#pragma once

#include <stdint.h>

struct IDirectInputA;
struct IDirectInputDeviceA;

namespace GA
{
	class Graphics;
	struct InputState;

	enum class InputMouseButton : uint8_t
	{
		Left,
		Right,
		Middle
	};

	enum class InputKeyboardKey : uint8_t
	{
		Escape,
		Space,
		Shift,
		W,
		A,
		S,
		D,
		Left,
		Right,
		Up,
		Down,

		Count
	};

	class Input
	{
	public:

		bool					create( Graphics& graphics );
		void					destroy();

		void					update();

		float					getMouseDeltaX() const;
		float					getMouseDeltaY() const;
		float					getMousePositionX() const;
		float					getMousePositionY() const;

		bool					isMouseButtonDown( InputMouseButton button ) const;
		bool					isMouseButtonUp( InputMouseButton button ) const;
		bool					wasMouseButtonPressed( InputMouseButton button ) const;
		bool					wasMouseButtonReleased( InputMouseButton button ) const;

		bool					isKeyboardKeyDown( InputKeyboardKey key ) const;
		bool					isKeyboardKeyUp( InputKeyboardKey key ) const;
		bool					wasKeyboardKeyPressed( InputKeyboardKey key ) const;
		bool					wasKeyboardKeyReleased( InputKeyboardKey key ) const;

	private:

		IDirectInputA*			m_pInputDevice	= nullptr;
		IDirectInputDeviceA*	m_pMouse		= nullptr;
		IDirectInputDeviceA*	m_pKeyboard		= nullptr;

		InputState*				m_pState		= nullptr;
	};
}

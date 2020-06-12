#pragma once

#include "../01-Window/SampleTypes.hpp"

struct IDirectInputA;
struct IDirectInputDeviceA;

namespace GamesAcademy
{
	class SampleWindow;
	struct SampleInputState;

	enum class SampleInputMouseButton : uint8
	{
		Left,
		Right,
		Middle
	};

	enum class SampleInputKeyboardKey : uint8
	{
		Escape,
		Space,
		Shift,
		F1,
		F2,
		F3,
		F4,
		F5,
		F6,
		F7,
		F8,
		F9,
		F10,
		F11,
		F12,
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

	class SampleInput
	{
	public:

								SampleInput();
								~SampleInput();

		bool					create( SampleWindow& window );
		void					destroy();

		void					update();

		SampleWindow&			getWindow() { return *m_pWindow; }

		float					getMouseDeltaX() const;
		float					getMouseDeltaY() const;
		float					getMousePositionX() const;
		float					getMousePositionY() const;

		bool					isMouseButtonDown( SampleInputMouseButton button ) const;
		bool					isMouseButtonUp( SampleInputMouseButton button ) const;
		bool					wasMouseButtonPressed( SampleInputMouseButton button ) const;
		bool					wasMouseButtonReleased( SampleInputMouseButton button ) const;

		bool					isKeyboardKeyDown( SampleInputKeyboardKey key ) const;
		bool					isKeyboardKeyUp( SampleInputKeyboardKey key ) const;
		bool					wasKeyboardKeyPressed( SampleInputKeyboardKey key ) const;
		bool					wasKeyboardKeyReleased( SampleInputKeyboardKey key ) const;

	private:

		SampleWindow*			m_pWindow		= nullptr;

		IDirectInputA*			m_pInputDevice	= nullptr;
		IDirectInputDeviceA*	m_pMouse		= nullptr;
		IDirectInputDeviceA*	m_pKeyboard		= nullptr;

		SampleInputState*		m_pState		= nullptr;
	};
}

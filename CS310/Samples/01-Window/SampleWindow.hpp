#pragma once

#include "SampleTypes.hpp"

struct HWND__;
using HWND = HWND__*;

namespace GamesAcademy
{
	class SampleWindow
	{
	public:

									SampleWindow();
									~SampleWindow();

		bool						create( const char* pWindowTitleUtf8, uint16 width, uint16 height );
		void						destroy();

		void						update();

		void						setPinMouse( bool enabled );

		bool						isOpen() const { return m_isOpen; }
		HWND						getHandle() const { return m_windowHandle; }
		int							getClientWidth() const { return m_clientWidth; }
		int							getClientHeight() const { return m_clientHeight; }

		void						showMessageBox( const wchar_t* pMessage ) const;

		bool						handleMessage( unsigned int messageCode, size_t wParam );

	private:

		HWND						m_windowHandle	= nullptr;
		wchar_t*					m_pWindowTitle	= nullptr;

		bool						m_isOpen		= false;
		bool						m_isMousePined	= false;
		int							m_clientWidth	= 0;
		int							m_clientHeight	= 0;
	};
}
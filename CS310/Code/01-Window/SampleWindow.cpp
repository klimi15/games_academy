#include "SampleWindow.hpp"

#include <Windows.h>

namespace GamesAcademy
{
	namespace SampleWindowInternal
	{
		static LRESULT CALLBACK		windowProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
	}

	SampleWindow::SampleWindow()
	{

	}

	SampleWindow::~SampleWindow()
	{
		destroy();
	}

	bool SampleWindow::create( const char* pWindowTitleUtf8, uint16 width, uint16 height )
	{
		const int windowTitleLength = MultiByteToWideChar( CP_UTF8, 0, pWindowTitleUtf8, -1, nullptr, 0 );
		m_pWindowTitle = new wchar_t[ windowTitleLength ];
		MultiByteToWideChar( CP_UTF8, 0, pWindowTitleUtf8, -1, m_pWindowTitle, windowTitleLength );

		const HINSTANCE	hInstance = GetModuleHandle( nullptr );

		WNDCLASSEXW windowClass = {};
		windowClass.cbSize			= sizeof( WNDCLASSEXW );
		windowClass.hInstance		= hInstance;
		windowClass.lpfnWndProc		= &SampleWindowInternal::windowProc;
		windowClass.lpszClassName	= L"GAWindowClass";
		windowClass.hbrBackground	= (HBRUSH)COLOR_WINDOW;
		windowClass.hCursor			= LoadCursor( nullptr, IDC_ARROW );

		HRESULT result = RegisterClassExW( &windowClass );
		if( FAILED( result ) )
		{
			showMessageBox( L"Can't register Class." );
			return false;
		}

		m_windowHandle = CreateWindow(
			L"GAWindowClass",
			m_pWindowTitle,
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			width,
			height,
			nullptr,
			nullptr,
			hInstance,
			nullptr
		);

		if( m_windowHandle == nullptr )
		{
			showMessageBox( L"Can't create Window." );
			return false;
		}

		SetWindowLongPtr( m_windowHandle, GWLP_USERDATA, (LONG_PTR)this );

		STARTUPINFO startupInfo = {};
		startupInfo.cb			= sizeof( startupInfo );
		startupInfo.dwFlags		= STARTF_USESHOWWINDOW;
		startupInfo.wShowWindow	= SW_SHOWMINNOACTIVE;

		ShowWindow( m_windowHandle, SW_SHOWNORMAL );
		UpdateWindow( m_windowHandle );

		RECT clientRect;
		GetClientRect( m_windowHandle, &clientRect );

		m_isOpen		= true;
		m_clientWidth	= (clientRect.right - clientRect.left);
		m_clientHeight	= (clientRect.bottom - clientRect.top);

		return true;
	}

	void SampleWindow::destroy()
	{
		if( m_windowHandle != nullptr )
		{
			DestroyWindow( m_windowHandle );
			m_windowHandle = nullptr;
		}

		if( m_pWindowTitle != nullptr )
		{
			delete[] m_pWindowTitle;
			m_pWindowTitle = nullptr;
		}
	}

	void SampleWindow::update()
	{
		MSG msg;
		if( PeekMessage( &msg, nullptr, 0U, 0U, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
	}


	void SampleWindow::setPinMouse( bool enabled )
	{
		if( enabled )
		{
			RECT clientRect;
			GetClientRect( m_windowHandle, &clientRect );

			POINT topLeftScreenPos;
			topLeftScreenPos.x = clientRect.left;
			topLeftScreenPos.y = clientRect.top;
			ClientToScreen( m_windowHandle, &topLeftScreenPos );

			POINT bottomRightScreenPos;
			bottomRightScreenPos.x = clientRect.right;
			bottomRightScreenPos.y = clientRect.bottom;
			ClientToScreen( m_windowHandle, &bottomRightScreenPos );

			RECT clipRect;
			clipRect.left	= topLeftScreenPos.x + 1;
			clipRect.top	= topLeftScreenPos.y + 1;
			clipRect.right	= bottomRightScreenPos.x - 1;
			clipRect.bottom	= bottomRightScreenPos.y - 1;
			ClipCursor( &clipRect );
		}
		else
		{
			ClipCursor( nullptr );
		}

		ShowCursor( enabled ? FALSE : TRUE );
		m_isMousePined = enabled;
	}

	void SampleWindow::showMessageBox( const wchar_t* pMessage ) const
	{
		MessageBoxW( m_windowHandle, pMessage, m_pWindowTitle, MB_ICONSTOP );
	}

	bool SampleWindow::handleMessage( unsigned int messageCode, size_t wParam )
	{
		switch( messageCode )
		{
		case WM_CLOSE:
			m_isOpen = false;
			return true;

		case WM_SIZE:
			{
				RECT clientRect;
				GetClientRect( m_windowHandle, &clientRect );

				m_clientWidth	= (clientRect.right - clientRect.left);
				m_clientHeight	= (clientRect.bottom - clientRect.top);
			}
			return true;

		case WM_ACTIVATE:
			{
				if( wParam != WA_INACTIVE )
				{
					setPinMouse( m_isMousePined );
				}
			}
			break;

		default:
			break;
		}

		return false;
	}

	LRESULT CALLBACK SampleWindowInternal::windowProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
	{
		SampleWindow* pWindow = (SampleWindow*)GetWindowLongPtr( hWnd, GWLP_USERDATA );
		if( pWindow != nullptr &&
			pWindow->handleMessage( message, wParam ) )
		{
			return 0u;
		}

		switch( message )
		{
		case WM_DESTROY:
			PostQuitMessage( 0 );
			break;

		default:
			return DefWindowProc( hWnd, message, wParam, lParam );
			break;
		}

		return 0;
	}
}
#pragma once

#include <cstdint>
#include <d3d11.h>
#include <windows.h>

namespace GamesAcademy
{
	class Graphics
	{
	public:

		bool						create( int windowWidth, int windowHeight, const char* pWindowTitleUtf8 );
		void						destroy();

		HWND						getWindowHandle() const { return m_windowHandle; }
		bool						isWindowOpen() const { return m_running; }
		void						closeWindow() { m_running = false; }

		float						getBackBufferWidth() const { return float( m_windowClientWidth ); }
		float						getBackBufferHeight() const { return float( m_windowClientHeight ); }

		void						beginFrame();
		void						endFrame();

		void						clear( uint32_t color );
		void						drawRect( float x, float y, float width, float height, uint32_t color );
		void						drawTriangle( float x1, float y1, float x2, float y2, float x3, float y3, uint32_t color );

	private:

		WCHAR						m_windowTitle[ 256u ]	= L"";
		HWND						m_windowHandle			= nullptr;
		int							m_windowClientWidth		= 0u;;
		int							m_windowClientHeight	= 0u;
		bool						m_running				= true;
		bool						m_sizeChanged			= false;

		IDXGISwapChain*				m_pSwapChain			= nullptr;
		ID3D11Device*				m_pDevice				= nullptr;
		ID3D11DeviceContext*		m_pContext				= nullptr;
		ID3D11RenderTargetView*		m_pBackBufferView		= nullptr;

		ID3D11VertexShader*			m_pVertexShader			= nullptr;
		ID3D11PixelShader*			m_pPixelShader			= nullptr;
		ID3D11InputLayout*			m_pInputLayout			= nullptr;
		ID3D11Buffer*				m_pVertexBuffer			= nullptr;
		ID3D11Buffer*				m_pConstantBuffer		= nullptr;

		bool						createWindow( int windowWidth, int windowHeight );
		void						destroyWindow();
		bool						handleWindowMessage( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
		static LRESULT CALLBACK		windowProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

		void						destroyDevice();
		bool						createDevice();

		bool						createResources();
		void						destroyResources();

		ID3D10Blob*					compileShader( const void* pCode, size_t codeSize, const char* pTarget );

		void						update();
		void						updateWindow();
		void						updateDevice();
		void						updateResources();
	};
}
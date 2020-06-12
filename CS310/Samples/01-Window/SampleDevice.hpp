#pragma once

struct ID3D11DepthStencilView;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11RenderTargetView;
struct ID3D11Texture2D;
struct IDXGISwapChain;

namespace GamesAcademy
{
	class SampleWindow;

	class SampleDevice
	{
	public:

									SampleDevice();
									~SampleDevice();

		bool						create( const SampleWindow& window, bool enableDepth );
		void						destroy();

		void						update();

		ID3D11DeviceContext*		beginFrame( float backBufferClearColor[ 4u ] );
		void						endFrame();

		void						applyBackBuffer( ID3D11DeviceContext* pContext ) const;

		const SampleWindow&			getWindow() const { return *m_pWindow; }
		ID3D11Device*				getDevice() { return m_pDevice; }

	private:

		const SampleWindow*			m_pWindow			= nullptr;

		IDXGISwapChain*				m_pSwapChain		= nullptr;
		ID3D11Device*				m_pDevice			= nullptr;
		ID3D11DeviceContext*		m_pContext			= nullptr;

		int							m_backBufferWidth	= 0;
		int							m_backBufferHeight	= 0;
		ID3D11RenderTargetView*		m_pBackBufferView	= nullptr;
		bool						m_enableDepth		= false;
		ID3D11Texture2D*			m_pDepthStencil		= nullptr;
		ID3D11DepthStencilView*		m_pDepthStencilView	= nullptr;
	};
}
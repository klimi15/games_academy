#include "SampleDevice.hpp"

#include "SampleWindow.hpp"

#include <d3d11.h>

namespace GamesAcademy
{
	SampleDevice::SampleDevice()
	{

	}

	SampleDevice::~SampleDevice()
	{
		destroy();
	}

	bool SampleDevice::create( const SampleWindow& window, bool enableDepth )
	{
		m_pWindow		= &window;
		m_enableDepth	= enableDepth;

		DXGI_SWAP_CHAIN_DESC swapDesc ={};
		swapDesc.BufferCount						= 2;
		swapDesc.BufferDesc.Format					= DXGI_FORMAT_R8G8B8A8_UNORM;
		swapDesc.BufferDesc.Width					= window.getClientWidth();
		swapDesc.BufferDesc.Height					= window.getClientHeight();
		swapDesc.BufferDesc.RefreshRate.Denominator	= 1;
		swapDesc.BufferDesc.RefreshRate.Numerator	= 60;
		swapDesc.BufferUsage						= DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapDesc.OutputWindow						= window.getHandle();
		swapDesc.SampleDesc.Count					= 1;
		swapDesc.SampleDesc.Quality					= 0;
		swapDesc.Windowed							= TRUE;
		swapDesc.SwapEffect							= DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapDesc.Flags								= DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		const D3D_FEATURE_LEVEL supportedLevels[] =
		{
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
			//D3D_FEATURE_LEVEL_10_1,
			//D3D_FEATURE_LEVEL_10_0,
			//D3D_FEATURE_LEVEL_9_3
		};

		UINT flags = D3D11_CREATE_DEVICE_SINGLETHREADED;
#ifdef _DEBUG
		flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		D3D_FEATURE_LEVEL currentLevel;
		HRESULT result = D3D11CreateDeviceAndSwapChain(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			flags,
			supportedLevels,
			ARRAY_COUNT( supportedLevels ),
			D3D11_SDK_VERSION,
			&swapDesc,
			&m_pSwapChain,
			&m_pDevice,
			&currentLevel,
			&m_pContext
		);

		if( FAILED( result ) )
		{
			window.showMessageBox( L"Failed to create Device." );
			return false;
		}

		update();

		return true;
	}

	void SampleDevice::destroy()
	{
		if( m_pDepthStencilView != nullptr )
		{
			m_pDepthStencilView->Release();
			m_pDepthStencilView = nullptr;
		}

		if( m_pDepthStencil != nullptr )
		{
			m_pDepthStencil->Release();
			m_pDepthStencil = nullptr;
		}


		if( m_pBackBufferView != nullptr )
		{
			m_pContext->OMSetRenderTargets( 0u, nullptr, nullptr );

			m_pBackBufferView->Release();
			m_pBackBufferView = nullptr;
		}

		if( m_pContext != nullptr )
		{
			m_pContext->Release();
			m_pContext = nullptr;
		}

		if( m_pDevice != nullptr )
		{
			m_pDevice->Release();
			m_pDevice = nullptr;
		}

		if( m_pSwapChain != nullptr )
		{
			m_pSwapChain->Release();
			m_pSwapChain = nullptr;
		}

		m_enableDepth		= false;
		m_backBufferWidth	= 0;
		m_backBufferHeight	= 0;
		m_pWindow			= nullptr;
	}

	void SampleDevice::update()
	{
		const int windowClientWidth = m_pWindow->getClientWidth();
		const int windowClientHeight = m_pWindow->getClientHeight();
		if( (m_backBufferWidth == windowClientWidth && m_backBufferHeight == windowClientHeight) ||
			windowClientWidth == 0 ||
			windowClientHeight == 0 )
		{
			return;
		}

		m_pContext->OMSetRenderTargets( 0u, nullptr, nullptr );

		if( m_pBackBufferView != nullptr )
		{
			m_pBackBufferView->Release();
			m_pBackBufferView = nullptr;
		}

		if( m_pDepthStencilView != nullptr )
		{
			m_pDepthStencilView->Release();
			m_pDepthStencilView = nullptr;
		}

		if( m_pDepthStencil != nullptr )
		{
			m_pDepthStencil->Release();
			m_pDepthStencil = nullptr;
		}

		HRESULT result = m_pSwapChain->ResizeBuffers( 0, windowClientWidth, windowClientHeight, DXGI_FORMAT_UNKNOWN, 0 );
		if( FAILED( result ) )
		{
			return;
		}

		{
			ID3D11Texture2D* pBackBuffer;
			result = m_pSwapChain->GetBuffer( 0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer );
			if( FAILED( result ) )
			{
				m_pWindow->showMessageBox( L"Could not get Back Buffer." );
				return;
			}

			result = m_pDevice->CreateRenderTargetView( pBackBuffer, nullptr, &m_pBackBufferView );
			pBackBuffer->Release();
			if( FAILED( result ) )
			{
				m_pWindow->showMessageBox( L"Failed to create Back Buffer View." );
				return;
			}
		}

		if( m_enableDepth )
		{
			D3D11_TEXTURE2D_DESC depthStencilDesc = {};
			depthStencilDesc.Width				= windowClientWidth;
			depthStencilDesc.Height				= windowClientHeight;
			depthStencilDesc.MipLevels			= 1u;
			depthStencilDesc.ArraySize			= 1u;
			depthStencilDesc.Format				= DXGI_FORMAT_D32_FLOAT;
			depthStencilDesc.SampleDesc.Count	= 1u;
			depthStencilDesc.Usage				= D3D11_USAGE_DEFAULT;
			depthStencilDesc.BindFlags			= D3D11_BIND_DEPTH_STENCIL;

			result = m_pDevice->CreateTexture2D( &depthStencilDesc, nullptr, &m_pDepthStencil );
			if( FAILED( result ) )
			{
				m_pWindow->showMessageBox( L"Failed to create Depth Stencil." );
				return;
			}

			result = m_pDevice->CreateDepthStencilView( m_pDepthStencil, nullptr, &m_pDepthStencilView );
			if( FAILED( result ) )
			{
				m_pWindow->showMessageBox( L"Failed to create Depth Stencil View." );
				return;
			}
		}

		m_backBufferWidth	= windowClientWidth;
		m_backBufferHeight	= windowClientHeight;
	}

	ID3D11DeviceContext* SampleDevice::beginFrame( float backBufferClearColor[ 4u ] )
	{
		m_pContext->ClearRenderTargetView( m_pBackBufferView, backBufferClearColor );

		if( m_pDepthStencilView != nullptr )
		{
			m_pContext->ClearDepthStencilView( m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0u );
		}

		applyBackBuffer( m_pContext );

		return m_pContext;
	}

	void SampleDevice::endFrame()
	{
		m_pSwapChain->Present( 1u, 0u );
	}

	void SampleDevice::applyBackBuffer( ID3D11DeviceContext* pContext ) const
	{
		pContext->OMSetRenderTargets( 1u, &m_pBackBufferView, m_pDepthStencilView );

		D3D11_VIEWPORT viewport = {};
		viewport.Width		= (float)m_backBufferWidth;
		viewport.Height		= (float)m_backBufferHeight;
		viewport.MaxDepth	= 1.0f;
		m_pContext->RSSetViewports( 1u, &viewport );
	}
}
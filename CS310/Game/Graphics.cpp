#include "Graphics.h"

#include "Types.h"

#include <d3d11.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <windows.h>

namespace GA
{
	namespace GraphicsInternal
	{
		static LRESULT CALLBACK		windowProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
	}

	bool Graphics::create( const char* pWindowTitleUtf8, uint16_t width, uint16_t height )
	{
		const int windowTitleLength = MultiByteToWideChar( CP_UTF8, 0, pWindowTitleUtf8, -1, nullptr, 0 );
		m_pWindowTitle = new wchar_t[ windowTitleLength ];
		MultiByteToWideChar( CP_UTF8, 0, pWindowTitleUtf8, -1, m_pWindowTitle, windowTitleLength );

		const HINSTANCE	hInstance = GetModuleHandle( nullptr );

		WNDCLASSEXW windowClass ={};
		windowClass.cbSize			= sizeof( WNDCLASSEXW );
		windowClass.hInstance		= hInstance;
		windowClass.lpfnWndProc		= &GraphicsInternal::windowProc;
		windowClass.lpszClassName	= L"GAWindowClass";
		windowClass.hbrBackground	= (HBRUSH)COLOR_WINDOW;
		windowClass.hCursor			= LoadCursor( nullptr, IDC_ARROW );

		HRESULT result = RegisterClassExW( &windowClass );
		if( FAILED( result ) )
		{
			destroy();
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
			destroy();
			showMessageBox( L"Can't create Window." );
			return false;
		}

		SetWindowLongPtr( m_windowHandle, GWLP_USERDATA, (LONG_PTR)this );

		ShowWindow( m_windowHandle, SW_SHOWNORMAL );
		UpdateWindow( m_windowHandle );

		RECT clientRect;
		GetClientRect( m_windowHandle, &clientRect );

		m_isOpen		= true;
		m_clientWidth	= (clientRect.right - clientRect.left);
		m_clientHeight	= (clientRect.bottom - clientRect.top);

		DXGI_SWAP_CHAIN_DESC swapDesc ={};
		swapDesc.BufferCount						= 2;
		swapDesc.BufferDesc.Format					= DXGI_FORMAT_R8G8B8A8_UNORM;
		swapDesc.BufferDesc.Width					= m_clientWidth;
		swapDesc.BufferDesc.Height					= m_clientHeight;
		swapDesc.BufferDesc.RefreshRate.Denominator	= 1;
		swapDesc.BufferDesc.RefreshRate.Numerator	= 60;
		swapDesc.BufferUsage						= DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapDesc.OutputWindow						= m_windowHandle;
		swapDesc.SampleDesc.Count					= 1;
		swapDesc.SampleDesc.Quality					= 0;
		swapDesc.Windowed							= TRUE;
		swapDesc.SwapEffect							= DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapDesc.Flags								= DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		const D3D_FEATURE_LEVEL supportedLevels[] =
		{
			D3D_FEATURE_LEVEL_11_1
		};

		UINT flags = D3D11_CREATE_DEVICE_SINGLETHREADED;
#ifdef _DEBUG
		flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		D3D_FEATURE_LEVEL currentLevel;
		result = D3D11CreateDeviceAndSwapChain(
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
			destroy();
			showMessageBox( L"Failed to create Device." );
			return false;
		}

		m_pContext->QueryInterface( &m_pContext1 );

		update();

		D3D11_BUFFER_DESC bufferDesc ={};
		bufferDesc.ByteWidth		= 4u * 1024u * 1024u;
		bufferDesc.Usage			= D3D11_USAGE_DYNAMIC;
		bufferDesc.BindFlags		= D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags	= D3D11_CPU_ACCESS_WRITE;

		result = m_pDevice->CreateBuffer( &bufferDesc, nullptr, &m_pConstantBuffer );
		if( FAILED( result ) )
		{
			destroy();
			showMessageBox( L"Failed to create Constant Buffer." );
			return false;
		}

		return true;
	}

	void Graphics::destroy()
	{
		while( m_pFirstShader != nullptr )
		{
			Shader* pShader = m_pFirstShader;
			m_pFirstShader = pShader->pNext;

			SAFE_RELEASE( pShader->pVertexLayout );
			SAFE_RELEASE( pShader->pVertexShader );
			SAFE_RELEASE( pShader->pPixelShader );
		}

		SAFE_RELEASE( m_pConstantBuffer );
		SAFE_RELEASE( m_pDepthStencilView );
		SAFE_RELEASE( m_pDepthStencil );
		SAFE_RELEASE( m_pBackBufferView );
		SAFE_RELEASE( m_pContext1 );
		SAFE_RELEASE( m_pContext );
		SAFE_RELEASE( m_pDevice );
		SAFE_RELEASE( m_pSwapChain );

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

	void Graphics::update()
	{
		MSG msg;
		if( PeekMessage( &msg, nullptr, 0U, 0U, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}

		const int windowClientWidth = m_clientWidth;
		const int windowClientHeight = m_clientHeight;
		if( (m_backBufferWidth == windowClientWidth && m_backBufferHeight == windowClientHeight) ||
			windowClientWidth == 0 ||
			windowClientHeight == 0 )
		{
			return;
		}

		m_pContext->OMSetRenderTargets( 0u, nullptr, nullptr );

		SAFE_RELEASE( m_pDepthStencilView );
		SAFE_RELEASE( m_pDepthStencil );
		SAFE_RELEASE( m_pBackBufferView );

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
				showMessageBox( L"Could not get Back Buffer." );
				return;
			}

			result = m_pDevice->CreateRenderTargetView( pBackBuffer, nullptr, &m_pBackBufferView );
			pBackBuffer->Release();
			if( FAILED( result ) )
			{
				showMessageBox( L"Failed to create Back Buffer View." );
				return;
			}
		}

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
			showMessageBox( L"Failed to create Depth Stencil." );
			return;
		}

		result = m_pDevice->CreateDepthStencilView( m_pDepthStencil, nullptr, &m_pDepthStencilView );
		if( FAILED( result ) )
		{
			showMessageBox( L"Failed to create Depth Stencil View." );
			return;
		}

		m_backBufferWidth	= windowClientWidth;
		m_backBufferHeight	= windowClientHeight;
	}

	GraphicsShaderHandle Graphics::createVertexShader( MemoryBlock data, GraphicsVertexFormat vertexFormat )
	{
		ID3DBlob* pShaderBlob = compileShader( data, "vertex_shader", "vs_5_0" );
		if( pShaderBlob == nullptr )
		{
			return false;
		}

		Shader* pShader = new Shader();

		HRESULT result = m_pDevice->CreateVertexShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), nullptr, &pShader->pVertexShader );
		if( FAILED( result ) )
		{
			pShader->pVertexShader->Release();
			delete pShader;

			pShaderBlob->Release();

			showMessageBox( L"Failed to create Vertex Shader." );
			return false;
		}

		switch( vertexFormat )
		{
		case GraphicsVertexFormat::Position3_Uv2:
			{
				const D3D11_INPUT_ELEMENT_DESC inputElements[] =
				{
					{ "POSITION",	0u, DXGI_FORMAT_R32G32B32_FLOAT,	0u, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0u },
					{ "TEXCOORD",	0u, DXGI_FORMAT_R32G32_FLOAT,		0u, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0u }
				};

				result = m_pDevice->CreateInputLayout( inputElements, ARRAY_COUNT( inputElements ), pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), &pShader->pVertexLayout );
			}
			break;
		}

		if( FAILED( result ) )
		{
			pShader->pVertexShader->Release();
			delete pShader;

			pShaderBlob->Release();

			showMessageBox( L"Failed to create Input Layout." );
			return false;
		}

		pShaderBlob->Release();

		pShader->pNext = m_pFirstShader;
		m_pFirstShader = pShader;

		return (GraphicsShaderHandle)pShader;
	}

	GraphicsShaderHandle Graphics::createPixelShader( MemoryBlock data )
	{
		ID3DBlob* pShaderBlob = compileShader( data, "pixel_shader", "ps_5_0" );
		if( pShaderBlob == nullptr )
		{
			return false;
		}

		Shader* pShader = new Shader();

		HRESULT result = m_pDevice->CreatePixelShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), nullptr, &pShader->pPixelShader );
		if( FAILED( result ) )
		{
			pShader->pPixelShader->Release();
			delete pShader;

			pShaderBlob->Release();

			showMessageBox( L"Failed to create Shader." );
			return false;
		}

		pShader->pNext = m_pFirstShader;
		m_pFirstShader = pShader;

		return (GraphicsShaderHandle)pShader;
	}

	ID3D11DeviceContext* Graphics::beginFrame( const float backBufferClearColor[ 4u ] )
	{
		m_pContext->ClearRenderTargetView( m_pBackBufferView, backBufferClearColor );
		m_pContext->ClearDepthStencilView( m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0u );

		m_pContext->OMSetRenderTargets( 1u, &m_pBackBufferView, m_pDepthStencilView );

		D3D11_VIEWPORT viewport = {};
		viewport.Width		= (float)m_backBufferWidth;
		viewport.Height		= (float)m_backBufferHeight;
		viewport.MaxDepth	= 1.0f;
		m_pContext->RSSetViewports( 1u, &viewport );

		D3D11_MAPPED_SUBRESOURCE constantData;
		m_pContext->Map( m_pConstantBuffer, 0u, D3D11_MAP_WRITE_DISCARD, 0u, &constantData );
		m_pConstantData		= (uint8_t*)constantData.pData;
		m_constantOffset	= 0u;

		return m_pContext;
	}

	void Graphics::endFrame()
	{
		m_pContext->Unmap( m_pConstantBuffer, 0u );
		m_pSwapChain->Present( 1u, 0u );
	}

	void Graphics::applyShader( GraphicsShaderHandle shaderHandle )
	{
		Shader* pShader = (Shader*)shaderHandle;
		if( pShader->pVertexShader != nullptr )
		{
			m_pContext->IASetInputLayout( pShader->pVertexLayout );
			m_pContext->VSSetShader( pShader->pVertexShader, nullptr, 0u );
		}
		else if( pShader->pPixelShader != nullptr )
		{
			m_pContext->PSSetShader( pShader->pPixelShader, nullptr, 0u );
		}
		else
		{
			OutputDebugStringW( L"Failed to set Shader.\n" );
		}
	}

	void* Graphics::applyVertexConstantData( uint32_t slotIndex, size_t size )
	{
		const UINT alignedSize = ALIGN_VALUE( size, 256u );
		const UINT consantSize = alignedSize / 16u;

		void* pData = m_pConstantData + (m_constantOffset * 16u);
		m_constantOffset += consantSize;

		m_pContext1->VSSetConstantBuffers1( slotIndex, 1u, &m_pConstantBuffer, &m_constantOffset, &consantSize );

		return pData;
	}

	void* Graphics::applyPixelConstantData( uint32_t slotIndex, size_t size )
	{
		const UINT alignedSize = ALIGN_VALUE( size, 256u );
		const UINT consantSize = alignedSize / 16u;

		void* pData = m_pConstantData + (m_constantOffset * 16u);
		m_constantOffset += consantSize;

		m_pContext1->PSSetConstantBuffers1( slotIndex, 1u, &m_pConstantBuffer, &m_constantOffset, &consantSize );

		return pData;
	}

	void Graphics::showMessageBox( const wchar_t* pMessage ) const
	{
		MessageBoxW( m_windowHandle, pMessage, m_pWindowTitle, MB_ICONSTOP );
	}

	bool Graphics::handleMessage( unsigned int messageCode, size_t wParam )
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

		default:
			break;
		}

		return false;
	}

	LRESULT CALLBACK GraphicsInternal::windowProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
	{
		Graphics* pGraphics = (Graphics*)GetWindowLongPtr( hWnd, GWLP_USERDATA );
		if( pGraphics != nullptr &&
			pGraphics->handleMessage( message, wParam ) )
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

	ID3D10Blob* Graphics::compileShader( const MemoryBlock& data, const char* pFilename, const char* pTarget )
	{
		ID3DBlob* pShaderBlob = nullptr;
		ID3DBlob* pErrorBlob = nullptr;
		UINT flags = 0u;
#ifdef _DEBUG
		flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
		HRESULT result = D3DCompile( data.pData, data.size, pFilename, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", pTarget, flags, 0u, &pShaderBlob, &pErrorBlob );
		if( FAILED( result ) )
		{
			if( pErrorBlob != nullptr )
			{
				WCHAR wideBuffer[ 2048u ];
				MultiByteToWideChar( CP_UTF8, 0, (const char*)pErrorBlob->GetBufferPointer(), -1, wideBuffer, ARRAY_COUNT( wideBuffer ) );

				showMessageBox( wideBuffer );

				pErrorBlob->Release();
				pErrorBlob = nullptr;
			}
			else
			{
				showMessageBox( L"Failed to compile Shader." );
			}

			return nullptr;
		}

		if( pErrorBlob != nullptr )
		{
			pErrorBlob->Release();
			pErrorBlob = nullptr;
		}

		return pShaderBlob;
	}
}

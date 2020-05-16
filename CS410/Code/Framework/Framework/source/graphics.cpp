#include "graphics.hpp"

#include <d3dcompiler.h>

#define ARRAY_COUNT( var ) (sizeof((var))/sizeof(*(var)))

namespace GamesAcademy
{
	struct GraphicsVertex
	{
		float		position[ 2u ];
		uint32_t	color;
	};

	struct GraphicsVertexConstantData
	{
		float		projection[ 4u ][ 4u ];
	};

	bool Graphics::create( int windowWidth, int windowHeight, const char* pm_windowTitleUtf8 )
	{
		MultiByteToWideChar( CP_UTF8, 0, pm_windowTitleUtf8, -1, m_windowTitle, 256 );

		if( !createWindow( windowWidth, windowHeight ) )
		{
			destroy();
			return false;
		}

		if( !createDevice() )
		{
			destroy();
			return false;
		}

		if( !createResources() )
		{
			destroy();
			return false;
		}

		return true;
	}

	void Graphics::destroy()
	{
		destroyResources();
		destroyDevice();
		destroyWindow();
	}

	void Graphics::beginFrame()
	{
		updateWindow();
		updateDevice();
		updateResources();

		m_pContext->OMSetRenderTargets( 1u, &m_pBackBufferView, nullptr );

		D3D11_VIEWPORT viewport ={};
		viewport.Width		= (float)m_windowClientWidth;
		viewport.Height		= (float)m_windowClientHeight;
		viewport.MaxDepth	= 1.0f;
		m_pContext->RSSetViewports( 1u, &viewport );

		m_pContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

		m_pContext->IASetInputLayout( m_pInputLayout );

		m_pContext->VSSetConstantBuffers( 0u, 1u, &m_pConstantBuffer );
		m_pContext->VSSetShader( m_pVertexShader, nullptr, 0u );

		m_pContext->PSSetShader( m_pPixelShader, nullptr, 0u );
	}

	void Graphics::endFrame()
	{
		m_pSwapChain->Present( 1u, 0u );
	}

	void Graphics::clear( uint32_t color )
	{
		const float floatColor[ 4u ] =
		{
			float( (color >>  0u) & 0xffu ) / 255.0f,
			float( (color >>  8u) & 0xffu ) / 255.0f,
			float( (color >> 16u) & 0xffu ) / 255.0f,
			float( (color >> 24u) & 0xffu ) / 255.0f
		};

		m_pContext->ClearRenderTargetView( m_pBackBufferView, floatColor );
	}

	void Graphics::drawRect( float x, float y, float width, float height, uint32_t color )
	{
		{
			D3D11_MAPPED_SUBRESOURCE mapData;
			m_pContext->Map( m_pVertexBuffer, 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mapData );

			GraphicsVertex* pVertices = (GraphicsVertex*)mapData.pData;
			pVertices[ 0u ].position[ 0u ]	= x;
			pVertices[ 0u ].position[ 1u ]	= y;
			pVertices[ 0u ].color			= color;

			pVertices[ 1u ].position[ 0u ]	= x + width;
			pVertices[ 1u ].position[ 1u ]	= y;
			pVertices[ 1u ].color			= color;

			pVertices[ 2u ].position[ 0u ]	= x + width;
			pVertices[ 2u ].position[ 1u ]	= y + height;
			pVertices[ 2u ].color			= color;

			pVertices[ 3u ].position[ 0u ]	= x + width;
			pVertices[ 3u ].position[ 1u ]	= y + height;
			pVertices[ 3u ].color			= color;

			pVertices[ 4u ].position[ 0u ]	= x;
			pVertices[ 4u ].position[ 1u ]	= y + height;
			pVertices[ 4u ].color			= color;

			pVertices[ 5u ].position[ 0u ]	= x;
			pVertices[ 5u ].position[ 1u ]	= y;
			pVertices[ 5u ].color			= color;

			m_pContext->Unmap( m_pVertexBuffer, 0u );
		}

		UINT stride = sizeof( GraphicsVertex );
		UINT offset = 0u;
		m_pContext->IASetVertexBuffers( 0u, 1u, &m_pVertexBuffer, &stride, &offset );

		m_pContext->Draw( 6u, 0u );
	}

	void Graphics::drawTriangle( float x1, float y1, float x2, float y2, float x3, float y3, uint32_t color )
	{
		{
			D3D11_MAPPED_SUBRESOURCE mapData;
			m_pContext->Map( m_pVertexBuffer, 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mapData );

			GraphicsVertex* pVertices = (GraphicsVertex*)mapData.pData;
			pVertices[ 0u ].position[ 0u ]	= x1;
			pVertices[ 0u ].position[ 1u ]	= y1;
			pVertices[ 0u ].color			= color;

			pVertices[ 1u ].position[ 0u ]	= x2;
			pVertices[ 1u ].position[ 1u ]	= y2;
			pVertices[ 1u ].color			= color;

			pVertices[ 2u ].position[ 0u ]	= x3;
			pVertices[ 2u ].position[ 1u ]	= y3;
			pVertices[ 2u ].color			= color;

			m_pContext->Unmap( m_pVertexBuffer, 0u );
		}

		UINT stride = sizeof( GraphicsVertex );
		UINT offset = 0u;
		m_pContext->IASetVertexBuffers( 0u, 1u, &m_pVertexBuffer, &stride, &offset );

		m_pContext->Draw( 3u, 0u );
	}

	bool Graphics::createWindow( int windowWidth, int windowHeight )
	{
		const HINSTANCE	hInstance = GetModuleHandle( nullptr );

		WNDCLASSEXW windowClass ={};
		windowClass.cbSize			= sizeof( WNDCLASSEXW );
		windowClass.hInstance		= hInstance;
		windowClass.lpfnWndProc		= &Graphics::windowProc;
		windowClass.lpszClassName	= L"GAWindowClass";
		windowClass.hbrBackground	= (HBRUSH)COLOR_WINDOW;
		windowClass.hCursor			= LoadCursor( nullptr, IDC_ARROW );

		HRESULT result = RegisterClassExW( &windowClass );
		if( FAILED( result ) )
		{
			MessageBoxW( nullptr, L"Can't register Class.", m_windowTitle, MB_ICONSTOP );
			return false;
		}

		m_windowHandle = CreateWindow(
			L"GAWindowClass",
			m_windowTitle,
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			windowWidth,
			windowHeight,
			nullptr,
			nullptr,
			hInstance,
			nullptr
		);

		if( m_windowHandle == nullptr )
		{
			MessageBoxW( nullptr, L"Can't create Window.", m_windowTitle, MB_ICONSTOP );
			return false;
		}

		SetWindowLongPtr( m_windowHandle, GWLP_USERDATA, (LONG_PTR)this );

		STARTUPINFO startupInfo ={};
		startupInfo.cb			= sizeof( startupInfo );
		startupInfo.dwFlags		= STARTF_USESHOWWINDOW;
		startupInfo.wShowWindow	= SW_SHOWMINNOACTIVE;

		ShowWindow( m_windowHandle, SW_SHOWNORMAL );
		UpdateWindow( m_windowHandle );

		RECT clientRect;
		GetClientRect( m_windowHandle, &clientRect );

		m_windowClientWidth		= (clientRect.right - clientRect.left);
		m_windowClientHeight	= (clientRect.bottom - clientRect.top);

		return true;
	}

	void Graphics::destroyWindow()
	{
		if( m_windowHandle != nullptr )
		{
			DestroyWindow( m_windowHandle );
			m_windowHandle = nullptr;
		}
	}

	bool Graphics::handleWindowMessage( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
	{
		switch( message )
		{
		case WM_CLOSE:
			m_running = false;
			return true;

		case WM_SIZE:
			{
				RECT clientRect;
				GetClientRect( m_windowHandle, &clientRect );

				m_sizeChanged	= true;
				m_windowClientWidth		= (clientRect.right - clientRect.left);
				m_windowClientHeight	= (clientRect.bottom - clientRect.top);
			}
			return true;

		default:
			break;
		}

		return false;
	}

	bool Graphics::createDevice()
	{
		DXGI_SWAP_CHAIN_DESC swapDesc ={};
		swapDesc.BufferCount						= 2;
		swapDesc.BufferDesc.Format					= DXGI_FORMAT_R8G8B8A8_UNORM;
		swapDesc.BufferDesc.Width					= m_windowClientWidth;
		swapDesc.BufferDesc.Height					= m_windowClientHeight;
		swapDesc.BufferDesc.RefreshRate.Denominator	= 1;
		swapDesc.BufferDesc.RefreshRate.Numerator	= 60;
		swapDesc.BufferUsage						= DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapDesc.OutputWindow						= m_windowHandle;
		swapDesc.SampleDesc.Count					= 1;
		swapDesc.SampleDesc.Quality					= 0;
		swapDesc.Windowed							= TRUE;
		swapDesc.SwapEffect							= DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapDesc.Flags								= DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		UINT flags = D3D11_CREATE_DEVICE_SINGLETHREADED;
#ifdef _DEBUG
		flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		D3D_FEATURE_LEVEL currentLevel;
		const D3D_FEATURE_LEVEL supportedLevel = D3D_FEATURE_LEVEL_11_0;
		HRESULT result = D3D11CreateDeviceAndSwapChain(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			flags,
			&supportedLevel,
			1u,
			D3D11_SDK_VERSION,
			&swapDesc,
			&m_pSwapChain,
			&m_pDevice,
			&currentLevel,
			&m_pContext
		);

		if( FAILED( result ) )
		{
			MessageBoxW( m_windowHandle, L"Failed to create Device.", m_windowTitle, MB_ICONSTOP );
			return false;
		}

		m_sizeChanged = true;
		updateDevice();

		return true;
	}

	void Graphics::destroyDevice()
	{
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
	}

	bool Graphics::createResources()
	{
		{
			static const char VertexShader[] = R"V0G0N(
				struct VertexInput
				{
					float2		position	: POSITION0;
					float4		color		: COLOR0;
				};

				struct VertexToPixel
				{
					float4		position	: SV_POSITION0;
					float4		color		: COLOR0;
				};

				cbuffer constants : register(b0)
				{
					float4x4	proj;
				};

				VertexToPixel main( VertexInput input )
				{
					float4 position = float4( input.position, 0.0f, 1.0 );
					position = mul( position, proj );

					VertexToPixel output;
					output.position		= position;
					output.color		= input.color;

					return output;
				}
			)V0G0N";

			ID3DBlob* pVertexShaderBlob = compileShader( VertexShader, sizeof( VertexShader ), "vs_5_0" );
			if( pVertexShaderBlob == nullptr )
			{
				return false;
			}

			HRESULT result = m_pDevice->CreateVertexShader( pVertexShaderBlob->GetBufferPointer(), pVertexShaderBlob->GetBufferSize(), nullptr, &m_pVertexShader );
			if( FAILED( result ) )
			{
				MessageBoxW( m_windowHandle, L"Failed to create Vertex Shader.", m_windowTitle, MB_ICONSTOP );
				return false;
			}

			const D3D11_INPUT_ELEMENT_DESC inputElements[] =
			{
				{ "POSITION",	0u, DXGI_FORMAT_R32G32_FLOAT,	0u, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0u },
				{ "COLOR",		0u, DXGI_FORMAT_R8G8B8A8_UNORM,	0u, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0u }
			};

			result = m_pDevice->CreateInputLayout( inputElements, ARRAY_COUNT( inputElements ), pVertexShaderBlob->GetBufferPointer(), pVertexShaderBlob->GetBufferSize(), &m_pInputLayout );
			if( FAILED( result ) )
			{
				pVertexShaderBlob->Release();
				MessageBoxW( m_windowHandle, L"Failed to create Input Layout.", m_windowTitle, MB_ICONSTOP );
				return false;
			}

			pVertexShaderBlob->Release();
		}

		{
			static const char PixelShader[] = R"V0G0N(
				struct VertexToPixel
				{
					float4		position	: SV_POSITION0;
					float4		color		: COLOR0;
				};

				struct PixelOutput
				{
					float4		color		: SV_TARGET0;
				};

				PixelOutput main( VertexToPixel input )
				{
					PixelOutput output;
					output.color		= input.color;

					return output;
				}
			)V0G0N";

			ID3DBlob* pPixelShaderBlob = compileShader( PixelShader, sizeof( PixelShader ), "ps_5_0" );
			if( pPixelShaderBlob == nullptr )
			{
				return false;
			}

			HRESULT result = m_pDevice->CreatePixelShader( pPixelShaderBlob->GetBufferPointer(), pPixelShaderBlob->GetBufferSize(), nullptr, &m_pPixelShader );
			if( FAILED( result ) )
			{
				pPixelShaderBlob->Release();
				MessageBoxW( m_windowHandle, L"Failed to create Pixel Shader.", m_windowTitle, MB_ICONSTOP );
				return false;
			}

			pPixelShaderBlob->Release();
		}

		{
			D3D11_BUFFER_DESC bufferDesc = {};
			bufferDesc.ByteWidth		= sizeof( GraphicsVertex ) * 6u;
			bufferDesc.Usage			= D3D11_USAGE_DYNAMIC;
			bufferDesc.BindFlags		= D3D11_BIND_VERTEX_BUFFER;
			bufferDesc.CPUAccessFlags	= D3D11_CPU_ACCESS_WRITE;

			HRESULT result = m_pDevice->CreateBuffer( &bufferDesc, nullptr, &m_pVertexBuffer );
			if( FAILED( result ) )
			{
				MessageBoxW( m_windowHandle, L"Failed to create Vertex Buffer.", m_windowTitle, MB_ICONSTOP );
				return false;
			}
		}

		{
			D3D11_BUFFER_DESC bufferDesc = {};
			bufferDesc.ByteWidth		= sizeof( float ) * 4 * 4;
			bufferDesc.Usage			= D3D11_USAGE_DYNAMIC;
			bufferDesc.BindFlags		= D3D11_BIND_CONSTANT_BUFFER;
			bufferDesc.CPUAccessFlags	= D3D11_CPU_ACCESS_WRITE;

			HRESULT result = m_pDevice->CreateBuffer( &bufferDesc, nullptr, &m_pConstantBuffer );
			if( FAILED( result ) )
			{
				MessageBoxW( m_windowHandle, L"Failed to create Constant Buffer.", m_windowTitle, MB_ICONSTOP );
				return false;
			}
		}

		return true;
	}

	void Graphics::destroyResources()
	{
		if( m_pConstantBuffer != nullptr )
		{
			m_pConstantBuffer->Release();
			m_pConstantBuffer = nullptr;
		}

		if( m_pVertexBuffer != nullptr )
		{
			m_pVertexBuffer->Release();
			m_pVertexBuffer = nullptr;
		}

		if( m_pPixelShader != nullptr )
		{
			m_pPixelShader->Release();
			m_pPixelShader = nullptr;
		}

		if( m_pInputLayout != nullptr )
		{
			m_pInputLayout->Release();
			m_pInputLayout = nullptr;
		}

		if( m_pVertexShader != nullptr )
		{
			m_pVertexShader->Release();
			m_pVertexShader = nullptr;
		}
	}

	ID3DBlob* Graphics::compileShader( const void* pCode, size_t codeSize, const char* pTarget )
	{
		ID3DBlob* pShaderBlob = nullptr;
		ID3DBlob* pErrorBlob = nullptr;
		HRESULT result = D3DCompile( pCode, codeSize, pTarget, nullptr, nullptr, "main", pTarget, D3DCOMPILE_DEBUG, 0u, &pShaderBlob, &pErrorBlob );
		if( FAILED( result ) )
		{
			if( pErrorBlob != nullptr )
			{
				MessageBoxA( m_windowHandle, (const char*)pErrorBlob->GetBufferPointer(), pTarget, MB_ICONSTOP );
				pErrorBlob->Release();
			}
			else
			{
				MessageBoxW( m_windowHandle, L"Failed to compile Shader.", m_windowTitle, MB_ICONSTOP );
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

	void Graphics::updateWindow()
	{
		MSG msg;
		if( PeekMessage( &msg, nullptr, 0U, 0U, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
	}

	void Graphics::updateDevice()
	{
		if( !m_sizeChanged )
		{
			return;
		}

		m_pContext->OMSetRenderTargets( 0u, nullptr, nullptr );

		if( m_pBackBufferView != nullptr )
		{
			m_pBackBufferView->Release();
			m_pBackBufferView = nullptr;
		}

		HRESULT result = m_pSwapChain->ResizeBuffers( 0, m_windowClientWidth, m_windowClientHeight, DXGI_FORMAT_UNKNOWN, 0 );
		if( FAILED( result ) )
		{
			return;
		}

		{
			ID3D11Texture2D* pBackBuffer;
			result = m_pSwapChain->GetBuffer( 0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer );
			if( FAILED( result ) )
			{
				MessageBoxW( m_windowHandle, L"Could not get Back Buffer.", m_windowTitle, MB_ICONSTOP );
				return;
			}

			result = m_pDevice->CreateRenderTargetView( pBackBuffer, nullptr, &m_pBackBufferView );
			pBackBuffer->Release();
			if( FAILED( result ) )
			{
				MessageBoxW( m_windowHandle, L"Failed to create Back Buffer View.", m_windowTitle, MB_ICONSTOP );
				return;
			}
		}

		m_sizeChanged = false;
	}

	void Graphics::updateResources()
	{
		D3D11_MAPPED_SUBRESOURCE mapData;
		m_pContext->Map( m_pConstantBuffer, 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mapData );

		GraphicsVertexConstantData* pConstantData = (GraphicsVertexConstantData*)mapData.pData;

		pConstantData->projection[ 0u ][ 0u ] = 2.0f / float( m_windowClientWidth );
		pConstantData->projection[ 1u ][ 0u ] = 0.0f;
		pConstantData->projection[ 2u ][ 0u ] = 0.0f;
		pConstantData->projection[ 3u ][ 0u ] = 0.0f;

		pConstantData->projection[ 0u ][ 1u ] = 0.0f;
		pConstantData->projection[ 1u ][ 1u ] = 2.0f / -float( m_windowClientHeight );
		pConstantData->projection[ 2u ][ 1u ] = 0.0f;
		pConstantData->projection[ 3u ][ 1u ] = 0.0f;

		pConstantData->projection[ 0u ][ 2u ] = 0.0f;
		pConstantData->projection[ 1u ][ 2u ] = 0.0f;
		pConstantData->projection[ 2u ][ 2u ] = 1.0f / (0.0f - 1.0f);
		pConstantData->projection[ 3u ][ 2u ] = 0.0f;

		pConstantData->projection[ 0u ][ 3u ] = -1.0f;
		pConstantData->projection[ 1u ][ 3u ] = 1.0f;
		pConstantData->projection[ 2u ][ 3u ] = 0.0f;
		pConstantData->projection[ 3u ][ 3u ] = 1.0f;

		m_pContext->Unmap( m_pConstantBuffer, 0u );
	}

	LRESULT CALLBACK Graphics::windowProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
	{
		Graphics* pSample = (Graphics*)GetWindowLongPtr( hWnd, GWLP_USERDATA );
		if( pSample != nullptr &&
			pSample->handleWindowMessage( hWnd, message, wParam, lParam ) )
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
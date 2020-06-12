#include "Sample03Box.hpp"

#include "Sample03Shader.hpp"
#include "Sample03Types.hpp"

#include <d3d11.h>
#include <math.h>

namespace GamesAcademy
{
	static const char* WindowTitle = u8"CS310 03-Box 🍩";
	static const int WindowWidth = 1280;
	static const int WindowHeight = 720;

	int Sample03Box::run()
	{
		if( !create() )
		{
			return 1;
		}

		m_running = true;
		while( m_running )
		{
			update();
			render();

			m_running &= m_window.isOpen();
		}

		destroy();

		return 0;
	}

	bool Sample03Box::create()
	{
		if( !m_window.create( WindowTitle, WindowWidth, WindowHeight ) )
		{
			destroy();
			return false;
		}

		if( !m_device.create( m_window, true ) )
		{
			destroy();
			return false;
		}

		if( !m_states.create( m_device ) )
		{
			destroy();
			return false;
		}

		if( !m_filesystem.create() )
		{
			destroy();
			return false;
		}

		if( !m_input.create( m_window ) )
		{
			destroy();
			return false;
		}

		m_timer.create();

		if( !createResources() )
		{
			destroy();
			return false;
		}

		return true;
	}

	void Sample03Box::destroy()
	{
		destroyResources();
		m_timer.destroy();
		m_input.destroy();
		m_filesystem.destroy();
		m_states.destroy();
		m_device.destroy();
		m_window.destroy();
	}

	bool Sample03Box::createResources()
	{
		{
			const D3D11_INPUT_ELEMENT_DESC inputElements[] =
			{
				{ "POSITION",	0u, DXGI_FORMAT_R32G32B32_FLOAT,	0u, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0u },
				{ "TEXCOORD",	0u, DXGI_FORMAT_R32G32_FLOAT,		0u, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0u }
			};

			const MemoryBlock vertexShader = Sample03Shader::getVertexShader();
			if( !m_vertexShader.createVertexShaderFromMemory(m_device, vertexShader, inputElements, ARRAY_COUNT( inputElements ) ) )
			{
				return false;
			}
		}

		{
			const MemoryBlock pixelShader = Sample03Shader::getPixelShader();
			if( !m_pixelShader.createPixelShaderFromMemory( m_device, pixelShader ) )
			{
				return false;
			}
		}

		{
			const Sample03Vertex vertices[] =
			{
				// front
				{ { -0.5f, -0.5f,  0.5f }, { 1.0f, 1.0f } },
				{ {  0.5f, -0.5f,  0.5f }, { 0.0f, 1.0f } },
				{ {  0.5f,  0.5f,  0.5f }, { 0.0f, 0.0f } },
				{ { -0.5f,  0.5f,  0.5f }, { 1.0f, 0.0f } },
				// back
				{ { -0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f } },
				{ {  0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f } },
				{ {  0.5f,  0.5f, -0.5f }, { 1.0f, 0.0f } },
				{ { -0.5f,  0.5f, -0.5f }, { 0.0f, 0.0f } },
				// left
				{ { -0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f } },
				{ { -0.5f,  0.5f, -0.5f }, { 1.0f, 0.0f } },
				{ { -0.5f,  0.5f,  0.5f }, { 0.0f, 0.0f } },
				{ { -0.5f, -0.5f,  0.5f }, { 0.0f, 1.0f } },
				// right
				{ {  0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f } },
				{ {  0.5f,  0.5f, -0.5f }, { 0.0f, 0.0f } },
				{ {  0.5f,  0.5f,  0.5f }, { 1.0f, 0.0f } },
				{ {  0.5f, -0.5f,  0.5f }, { 1.0f, 1.0f } },
				// top
				{ { -0.5f,  0.5f, -0.5f }, { 1.0f, 0.0f } },
				{ {  0.5f,  0.5f, -0.5f }, { 0.0f, 0.0f } },
				{ {  0.5f,  0.5f,  0.5f }, { 0.0f, 1.0f } },
				{ { -0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f } },
				// bottom
				{ { -0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f } },
				{ {  0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f } },
				{ {  0.5f, -0.5f,  0.5f }, { 0.0f, 1.0f } },
				{ { -0.5f, -0.5f,  0.5f }, { 1.0f, 1.0f } }
			};

			D3D11_BUFFER_DESC bufferDesc = {};
			bufferDesc.ByteWidth	= sizeof( vertices );
			bufferDesc.Usage		= D3D11_USAGE_IMMUTABLE;
			bufferDesc.BindFlags	= D3D11_BIND_VERTEX_BUFFER;

			D3D11_SUBRESOURCE_DATA bufferData = {};
			bufferData.pSysMem = vertices;

			HRESULT result = m_device.getDevice()->CreateBuffer( &bufferDesc, &bufferData, &m_pVertexBuffer );
			if( FAILED( result ) )
			{
				m_window.showMessageBox( L"Failed to create Vertex Buffer." );
				return false;
			}
		}

		{
			const uint16 indices[] =
			{
				// front
				 0u,  1u,  2u,
				 2u,  3u,  0u,
				// back
				 7u,  6u,  5u,
				 5u,  4u,  7u,
				// left
				11u, 10u,  9u,
				 9u,  8u, 11u,
				// right
				12u, 13u, 14u,
				14u, 15u, 12u,
				// top
				19u, 18u, 17u,
				17u, 16u, 19u,
				// bottom
				20u, 21u, 22u,
				22u, 23u, 20u
			};

			D3D11_BUFFER_DESC bufferDesc = {};
			bufferDesc.ByteWidth	= sizeof( indices );
			bufferDesc.Usage		= D3D11_USAGE_IMMUTABLE;
			bufferDesc.BindFlags	= D3D11_BIND_INDEX_BUFFER;

			D3D11_SUBRESOURCE_DATA bufferData = {};
			bufferData.pSysMem = indices;

			HRESULT result = m_device.getDevice()->CreateBuffer( &bufferDesc, &bufferData, &m_pIndexBuffer );
			if( FAILED( result ) )
			{
				m_window.showMessageBox( L"Failed to create Index Buffer." );
				return false;
			}

			m_indexCount = ARRAY_COUNT( indices );
		}

		if( !m_vertexConstants.create( m_device, sizeof( float ) * 4u * 4u ) ||
			!m_texture.createFromFile( m_device, m_filesystem, "Content/checker_rgb.dds" ) )
		{
			return false;
		}

		{
			const XMVECTOR cameraPosition		= XMVectorSet( 0.0f, 2.0f, -5.0f, 0.0f );
			const XMFLOAT2 cameraOrientation	= { XM_PI / 8.0f, 0.0f };
			m_camera.create( m_input, true, cameraPosition, cameraOrientation, 10.0f );
		}

		return true;
	}

	void Sample03Box::destroyResources()
	{
		m_camera.destroy();

		m_texture.destroy();
		m_vertexConstants.destroy();

		if( m_pIndexBuffer != nullptr )
		{
			m_pIndexBuffer->Release();
			m_pIndexBuffer = nullptr;
		}

		if( m_pVertexBuffer != nullptr )
		{
			m_pVertexBuffer->Release();
			m_pVertexBuffer = nullptr;
		}

		m_pixelShader.destroy();
		m_vertexShader.destroy();
	}

	void Sample03Box::update()
	{
		m_window.update();
		m_device.update();
		m_input.update();
		m_timer.update();
		m_camera.update( m_timer );

		if( m_input.wasKeyboardKeyPressed( SampleInputKeyboardKey::Escape ) )
		{
			m_running = false;
		}
	}

	void Sample03Box::render()
	{
		ID3D11DeviceContext* pContext = m_device.beginFrame( m_backgroundColor );

		{
			const XMMATRIX modelMatrix		= XMMatrixRotationY( float( m_timer.getGameTime() ) );
			const XMMATRIX viewMatrix		= m_camera.getViewMatrix();
			const XMMATRIX projectionMatrix	= XMMatrixPerspectiveFovLH( XM_PI / 4.0f, float( m_window.getClientWidth() ) / float( m_window.getClientHeight() ), 0.0001f, 100.0f );

			XMMATRIX mvpMatrix = modelMatrix;
			mvpMatrix *= viewMatrix;
			mvpMatrix *= projectionMatrix;
			const XMMATRIX mvpTransposeMatrix = XMMatrixTranspose( mvpMatrix );

			Sample03ConstantData* pConstantData = m_vertexConstants.map< Sample03ConstantData >( pContext );
			XMStoreFloat4x4( &pConstantData->mvp, mvpTransposeMatrix );
			m_vertexConstants.unmap( pContext );
		}

		UINT stride = sizeof( Sample03Vertex );
		UINT offset = 0u;
		pContext->IASetVertexBuffers( 0u, 1u, &m_pVertexBuffer, &stride, &offset );
		pContext->IASetIndexBuffer( m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0u );
		pContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

		m_vertexConstants.applyToVertexShader( pContext, 0u );
		m_vertexShader.apply( pContext );

		m_states.applySamplerStateToPixelShader( pContext, 0u, SampleSamplerFilter::Linear );
		m_texture.applyToPixelShader( pContext, 0u );
		m_pixelShader.apply( pContext );

		pContext->DrawIndexed( m_indexCount, 0u, 0u );

		m_device.endFrame();
	}
}

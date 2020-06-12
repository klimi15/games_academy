#include "Sample02Triangle.hpp"

#include "Sample02Shader.hpp"
#include "Sample02Types.hpp"

#include <d3d11.h>
#include <math.h>

namespace GamesAcademy
{
	static const char* WindowTitle = u8"CS310 02-Triangle 🍩";
	static const int WindowWidth = 1280;
	static const int WindowHeight = 720;

	int Sample02Triangle::run()
	{
		if( !create() )
		{
			return 1;
		}

		while( m_window.isOpen() )
		{
			update();
			render();
		}

		destroy();

		return 0;
	}

	bool Sample02Triangle::create()
	{
		if( !m_window.create( WindowTitle, WindowWidth, WindowHeight ) )
		{
			destroy();
			return false;
		}

		if( !m_device.create( m_window, false ) )
		{
			destroy();
			return false;
		}

		if( !m_states.create( m_device ) )
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

	void Sample02Triangle::destroy()
	{
		destroyResources();
		m_timer.destroy();
		m_states.destroy();
		m_device.destroy();
		m_window.destroy();
	}

	bool Sample02Triangle::createResources()
	{
		{
			const D3D11_INPUT_ELEMENT_DESC inputElements[] =
			{
				{ "POSITION",	0u, DXGI_FORMAT_R32G32_FLOAT,		0u, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0u },
				{ "COLOR",		0u, DXGI_FORMAT_R32G32B32_FLOAT,	0u, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0u }
			};

			const MemoryBlock vertexShader = Sample02Shader::getVertexShader();
			if( !m_vertexShader.createVertexShaderFromMemory( m_device, vertexShader, inputElements, ARRAY_COUNT( inputElements ) ) )
			{
				return false;
			}
		}

		{
			const MemoryBlock pixelShader = Sample02Shader::getPixelShader();
			if( !m_pixelShader.createPixelShaderFromMemory( m_device, pixelShader ) )
			{
				return false;
			}
		}

		{
			Sample02Vertex vertices[] =
			{
				{ {  0.0f,  0.5f }, { 1.0f, 0.0f, 0.0f } },
				{ {  0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f } },
				{ { -0.5f, -0.5f }, { 0.0f, 0.0f, 1.0f } }
			};

			D3D11_BUFFER_DESC bufferDesc = {};
			bufferDesc.ByteWidth			= sizeof( vertices );
			bufferDesc.Usage				= D3D11_USAGE_IMMUTABLE;
			bufferDesc.BindFlags			= D3D11_BIND_VERTEX_BUFFER;

			D3D11_SUBRESOURCE_DATA bufferData = {};
			bufferData.pSysMem = vertices;

			const HRESULT result = m_device.getDevice()->CreateBuffer( &bufferDesc, &bufferData, &m_pVertexBuffer );
			if( FAILED( result ) )
			{
				m_window.showMessageBox( L"Failed to create Vertex Buffer." );
				return false;
			}
		}

		return true;
	}

	void Sample02Triangle::destroyResources()
	{
		if( m_pVertexBuffer != nullptr )
		{
			m_pVertexBuffer->Release();
			m_pVertexBuffer = nullptr;
		}

		m_pixelShader.destroy();
		m_vertexShader.destroy();
	}

	void Sample02Triangle::update()
	{
		m_window.update();
		m_device.update();
		m_timer.update();
	}

	void Sample02Triangle::render()
	{
		ID3D11DeviceContext* pContext = m_device.beginFrame( m_backgroundColor );

		UINT stride = sizeof( Sample02Vertex );
		UINT offset = 0u;
		pContext->IASetVertexBuffers( 0u, 1u, &m_pVertexBuffer, &stride, &offset );
		pContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

		if( (size_t)m_timer.getGameTime() % 4 >= 2 )
		{
			m_states.applyRasterizerState( pContext, SampleFillMode::Wireframe );
		}
		else
		{
			m_states.applyRasterizerState( pContext, SampleFillMode::Solid );
		}

		m_vertexShader.apply( pContext );
		m_pixelShader.apply( pContext );

		pContext->Draw( 3u, 0u );

		m_device.endFrame();
	}
}

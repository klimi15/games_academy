#include "Model.h"

#include "Types.h"
#include "Graphics.h"

#include <d3d11.h>

namespace GA
{
	bool Model::createPlane( Graphics& graphics )
	{
		GraphicsVertexPosition3_Uv2 vertices[] =
		{
			{ {  0.0f,  0.5f, 0.0f }, { 1.0f, 0.0f } },
			{ {  0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f } },
			{ { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f } }
		};

		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.ByteWidth			= sizeof( vertices );
		bufferDesc.Usage				= D3D11_USAGE_IMMUTABLE;
		bufferDesc.BindFlags			= D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA bufferData = {};
		bufferData.pSysMem = vertices;

		HRESULT result = graphics.getDevice()->CreateBuffer( &bufferDesc, &bufferData, &m_pVertexBuffer );
		if( FAILED( result ) )
		{
			graphics.showMessageBox( L"Failed to create Vertex Buffer." );
			return false;
		}

		return true;
	}

	void Model::destroy()
	{
		SAFE_RELEASE( m_pVertexBuffer );
	}

	void Model::render( Graphics& graphics, ID3D11DeviceContext* pContext )
	{
		const UINT stride = sizeof( GraphicsVertexPosition3_Uv2 );
		const UINT offset = 0u;
		pContext->IASetVertexBuffers( 0u, 1u, &m_pVertexBuffer, &stride, &offset );
		pContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
		pContext->Draw( 3u, 0u );
	}
}

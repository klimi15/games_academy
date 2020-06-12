#include "Sample05Tessellation.hpp"

//#include "Sample05Types.hpp"

#include "../Shader/terrain_shader_types.hpp"

#include <d3d11.h>
#include <math.h>

namespace GamesAcademy
{
	static const char* WindowTitle = u8"CS310 05-Tessellation 🍩";
	static const int WindowWidth = 1280;
	static const int WindowHeight = 720;

	int Sample05Tessellation::run()
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

	bool Sample05Tessellation::create()
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

	void Sample05Tessellation::destroy()
	{
		destroyResources();
		m_timer.destroy();
		m_input.destroy();
		m_filesystem.destroy();
		m_states.destroy();
		m_device.destroy();
		m_window.destroy();
	}

	bool Sample05Tessellation::createResources()
	{
		m_lightDirection	= XMVectorSet( -0.506600857f, 0.301105350f, 0.807893097f, 0.0f );

		{
			const D3D11_INPUT_ELEMENT_DESC inputElements[] =
			{
				{ "POSITION",	0u, DXGI_FORMAT_R32G32B32_FLOAT,	0u, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0u },
				{ "TEXCOORD",	0u, DXGI_FORMAT_R32G32_FLOAT,		0u, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0u },
				{ "NORMAL",		0u, DXGI_FORMAT_R32G32B32_FLOAT,	0u, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0u },
				{ "TANGENT",	0u, DXGI_FORMAT_R32G32B32A32_FLOAT,	0u, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0u }
			};

			if( !m_terrainVertexShader.createVertexShaderFromFile(m_device, m_filesystem, "Shader/terrain.vs.hlsl", inputElements, ARRAY_COUNT( inputElements ) ) )
			{
				return false;
			}
		}

		{
			const D3D11_INPUT_ELEMENT_DESC inputElements[] =
			{
				{ "POSITION",	0u, DXGI_FORMAT_R32G32B32_FLOAT,	0u, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0u }
			};

			if( !m_skyboxVertexShader.createVertexShaderFromFile( m_device, m_filesystem, "Shader/skybox.vs.hlsl", inputElements, ARRAY_COUNT( inputElements ) ) )
			{
				return false;
			}
		}

		if( !m_terrainPixelShader.createPixelShaderFromFile( m_device, m_filesystem, "Shader/terrain.ps.hlsl" ) ||
			!m_skyboxPixelShader.createPixelShaderFromFile( m_device, m_filesystem, "Shader/skybox.ps.hlsl" ) )
		{
			return false;
		}

		if( !m_terrainHullShader.createHullShaderFromFile( m_device, m_filesystem, "Shader/terrain.hs.hlsl" ) ||
			!m_terrainDomainShader.createDomainShaderFromFile( m_device, m_filesystem, "Shader/terrain.ds.hlsl" ) )
		{
			return false;
		}

		if( !m_vertexViewConstants.create( m_device, sizeof( VertexViewConstantData ) ) ||
			!m_vertexModelConstants.create( m_device, sizeof( VertexModelConstantData ) ) ||
			!m_hullViewConstants.create( m_device, sizeof( TerrainHullConstants ) ) ||
			!m_pixelLightConstants.create( m_device, sizeof( PixelLightConstants ) ) )
		{
			return false;
		}

		{
			const size_t terrainSize	= 4096u;
			const size_t quadSize		= 64u;
			const size_t subdivisions	= terrainSize / quadSize;
			const size_t vertexCount	= subdivisions * subdivisions * 4u;

			SampleModelVertex* pVertices = new SampleModelVertex[ vertexCount ];
			memset( pVertices, 0, sizeof( SampleModelVertex ) * vertexCount );

			const float baseOffset		= float( subdivisions ) / -2.0f;
			const float quadSizeFloat	= float( quadSize );
			const float texCoordQuad	= 1.0f / float( subdivisions );
			for( size_t y = 0u; y < subdivisions; ++y )
			{
				SampleModelVertex* pLineStartVertices = &pVertices[ y * subdivisions * 4u ];

				for( size_t x = 0u; x < subdivisions; ++x )
				{
					SampleModelVertex* pQuadStartVertices = &pLineStartVertices[ x * 4u ];

					const float x0	= float( x );
					const float x1	= float( x ) + 1.0f;
					const float y0	= float( y );
					const float y1	= float( y ) + 1.0f;

					pQuadStartVertices[ 0u ].postion.x	= (baseOffset + x0) * quadSizeFloat;
					pQuadStartVertices[ 0u ].postion.z	= (baseOffset + y0) * quadSizeFloat;
					pQuadStartVertices[ 1u ].postion.x	= (baseOffset + x1) * quadSizeFloat;
					pQuadStartVertices[ 1u ].postion.z	= (baseOffset + y0) * quadSizeFloat;
					pQuadStartVertices[ 2u ].postion.x	= (baseOffset + x0) * quadSizeFloat;
					pQuadStartVertices[ 2u ].postion.z	= (baseOffset + y1) * quadSizeFloat;
					pQuadStartVertices[ 3u ].postion.x	= (baseOffset + x1) * quadSizeFloat;
					pQuadStartVertices[ 3u ].postion.z	= (baseOffset + y1) * quadSizeFloat;

					pQuadStartVertices[ 0u ].texCoord.x	= x0 * texCoordQuad;
					pQuadStartVertices[ 0u ].texCoord.y	= y0 * texCoordQuad;
					pQuadStartVertices[ 1u ].texCoord.x	= x1 * texCoordQuad;
					pQuadStartVertices[ 1u ].texCoord.y	= y0 * texCoordQuad;
					pQuadStartVertices[ 2u ].texCoord.x	= x0 * texCoordQuad;
					pQuadStartVertices[ 2u ].texCoord.y	= y1 * texCoordQuad;
					pQuadStartVertices[ 3u ].texCoord.x	= x1 * texCoordQuad;
					pQuadStartVertices[ 3u ].texCoord.y	= y1 * texCoordQuad;
				}
			}

			const MemoryBlock vertexData = { pVertices, sizeof( SampleModelVertex ) * vertexCount };
			if( !m_terrainModel.createFromMemory( m_device, vertexData, { nullptr, 0u }, vertexCount, sizeof( SampleModelVertex ) ) )
			{
				return false;
			}

			delete[] pVertices;
		}

		if( !m_shadowMap.createEmptyShadowMap( m_device, 2048u, 2048u ) ||
			!m_terrainSandMaterial.createFromFiles( m_device, m_filesystem, "Content/terrain/sand" ) ||
			!m_terrainGrassMaterial.createFromFiles( m_device, m_filesystem, "Content/terrain/grass" ) ||
			!m_terrainRockMaterial.createFromFiles( m_device, m_filesystem, "Content/terrain/rock" ) ||
			!m_terrainHeightMap.createFromFile(m_device, m_filesystem, "Content/terrain/height.dds" ) ||
			!m_terrainBlendMap.createFromFile(m_device, m_filesystem, "Content/terrain/material_blend.dds") ||
			!m_skyboxModel.createFromFile( m_device, m_filesystem, "Content/skybox/skybox.obj" ) ||
			!m_environmentMap.createFromFile( m_device, m_filesystem, "Content/skybox/skybox_sea.dds" ) )
		{
			return false;
		}

		{
			const XMVECTOR cameraPosition		= XMVectorSet( 0.739075f, 1988.95422f, 0.176270f, 0.0f );
			const XMFLOAT2 cameraOrientation	= { 0.115978763f, 0.0f };
			m_camera.create( m_input, true, cameraPosition, cameraOrientation, 100.0f );
		}

		return true;
	}

	void Sample05Tessellation::destroyResources()
	{
		m_camera.destroy();

		m_skyboxModel.destroy();
		m_environmentMap.destroy();

		m_terrainBlendMap.destroy();
		m_terrainHeightMap.destroy();
		m_terrainSandMaterial.destroy();
		m_terrainGrassMaterial.destroy();
		m_terrainRockMaterial.destroy();
		m_terrainModel.destroy();

		m_shadowMap.destroy();

		m_pixelLightConstants.destroy();
		m_hullViewConstants.destroy();
		m_vertexModelConstants.destroy();
		m_vertexViewConstants.destroy();

		m_terrainPixelShader.destroy();
		m_terrainHullShader.destroy();
		m_terrainDomainShader.destroy();
		m_terrainVertexShader.destroy();
		m_skyboxPixelShader.destroy();
		m_skyboxVertexShader.destroy();
	}

	void Sample05Tessellation::update()
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

		if( m_input.wasKeyboardKeyPressed( SampleInputKeyboardKey::Space ) )
		{
			m_wireframe = !m_wireframe;
		}

		if (m_input.wasKeyboardKeyPressed( SampleInputKeyboardKey::F1 ))
		{
			m_tessellationFactor = clamp( 0.0f, 5.0f, m_tessellationFactor - 0.05f );
		}
		else if( m_input.wasKeyboardKeyPressed( SampleInputKeyboardKey::F2 ) )
		{
			m_tessellationFactor = clamp( 0.0f, 5.0f, m_tessellationFactor + 0.05f );
		}

		if( m_input.isMouseButtonDown( SampleInputMouseButton::Left ) )
		{
			m_lightDirection = -XMVector3Normalize( m_camera.getForward() );
		}
	}

	void Sample05Tessellation::render()
	{
		ID3D11DeviceContext* pContext = m_device.beginFrame( m_backgroundColor );

		{
			float3 cameraPosition;
			float3 lightDirection;
			XMStoreFloat3( &cameraPosition, m_camera.getPosition() );
			XMStoreFloat3( &lightDirection, m_lightDirection );
			const float lightIntensity		= 2.0f;
			const float3 lightColor			= { 1.0f, 1.0f, 1.0f };
			const float2 shadowTexelSize	= { 1.0f / m_shadowMap.getWidth(), 1.0f / m_shadowMap.getHeight() };

			PixelLightConstants* pConstantData = m_pixelLightConstants.map< PixelLightConstants >( pContext );
			fillPixelLightConstants( *pConstantData, cameraPosition, lightDirection, lightIntensity, lightColor, shadowTexelSize );
			m_pixelLightConstants.unmap( pContext );
		}

		{
			XMFLOAT3 cameraPosition;
			XMStoreFloat3( &cameraPosition, m_camera.getPosition() );

			TerrainHullConstants* pConstantData = m_hullViewConstants.map< TerrainHullConstants >( pContext );
			fillTerrainHullConstants( pConstantData, cameraPosition, m_tessellationFactor );
			m_hullViewConstants.unmap( pContext );
		}

		pContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

		m_vertexViewConstants.applyToVertexShader( pContext, 0u );
		m_vertexModelConstants.applyToVertexShader( pContext, 1u );

		m_vertexViewConstants.applyToDomainShader( pContext, 0u );
		m_vertexModelConstants.applyToDomainShader( pContext, 1u );

		m_pixelLightConstants.applyToPixelShader( pContext, 0u );

		m_states.applySamplerStateToPixelShader( pContext, 0u, SampleSamplerFilter::Anisotropic );
		m_states.applySamplerStateToPixelShader( pContext, 1u, SampleSamplerFilter::Point, SampleSamplerAddressMode::Clamp );
		m_states.applySamplerStateToPixelShader( pContext, 2u, SampleSamplerFilter::Linear, SampleSamplerAddressMode::Clamp );

		XMMATRIX shadowMatrix;
		{
			m_shadowMap.applyRenderTargetAndClear( pContext );

			const XMVECTOR cameraPosition	= XMVectorSubtract( XMVectorAdd( m_camera.getPosition(), XMVectorScale( m_camera.getForward(), 500.0f ) ), XMVectorScale( m_lightDirection, -500.0f ) );
			const XMMATRIX view				= XMMatrixLookToLH( cameraPosition, -m_lightDirection, XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0 ) );
			const XMMATRIX projection		= XMMatrixOrthographicOffCenterLH( -1000.0f, 1000.0f, -1000.0, 1000.0f, 1.0f, 5000.0f );

			shadowMatrix					= view * projection;

			renderScene( pContext, view, projection, XMMatrixIdentity(), true );
		}

		m_states.applyRasterizerState( pContext, m_wireframe ? SampleFillMode::Wireframe : SampleFillMode::Solid );

		{
			m_device.applyBackBuffer( pContext );

			m_shadowMap.applyToPixelShader( pContext, 0u );

			const XMMATRIX view				= m_camera.getViewMatrix();
			const XMMATRIX projection		= XMMatrixPerspectiveFovLH( XM_PI / 4.0f, float( m_window.getClientWidth() ) / float( m_window.getClientHeight() ), 1.0f, 10000.0f );

			renderScene( pContext, view, projection, shadowMatrix, false );

			// Clear Shadow Map slot
			ID3D11ShaderResourceView* pNullResource = nullptr;
			pContext->PSSetShaderResources( 0u, 1u, &pNullResource );
		}

		m_states.applyRasterizerState( pContext, SampleFillMode::Solid );

		{
			m_skyboxVertexShader.apply( pContext );

			m_environmentMap.applyToPixelShader( pContext, 0u );
			m_skyboxPixelShader.apply( pContext );

			const XMMATRIX modelMatrix = XMMatrixScaling( 4500.0f, 4500.0f, 4500.0f ) * XMMatrixTranslation( 0.0f, 1500.0f, 0.0f );
			renderModel(pContext, m_skyboxModel, modelMatrix );
		}

		m_device.endFrame();
	}

	void Sample05Tessellation::renderScene( ID3D11DeviceContext* pContext, const XMMATRIX& view, const XMMATRIX& projection, const XMMATRIX& shadowMatrix, bool shadowMap )
	{
		{
			VertexViewConstantData* pConstantData = m_vertexViewConstants.map< VertexViewConstantData >( pContext );
			XMStoreFloat4x4( &pConstantData->viewProjection, XMMatrixTranspose( view * projection ) );
			XMStoreFloat4x4( &pConstantData->shadowViewProjection, XMMatrixTranspose( shadowMatrix ) );
			m_vertexViewConstants.unmap( pContext );
		}

		{
			m_hullViewConstants.applyToHullShader( pContext, 0u );

			const size_t shaderMode = (shadowMap ? TERRAIN_MODE_SHADOW : TERRAIN_MODE_DEFAULT);
			m_terrainVertexShader.apply( pContext, shaderMode );
			m_terrainHullShader.apply( pContext, shaderMode );
			m_terrainDomainShader.apply( pContext, shaderMode );
			m_terrainPixelShader.apply( pContext, shaderMode );

			m_terrainHeightMap.applyToDomainShader( pContext, 0u );
			m_states.applySamplerStateToDomainShader( pContext, 0u, SampleSamplerFilter::Linear, SampleSamplerAddressMode::Clamp );

			if( !shadowMap )
			{
				m_terrainBlendMap.applyToPixelShader( pContext, 2u );
				m_terrainSandMaterial.apply( pContext, 3u );
				m_terrainGrassMaterial.apply( pContext, 7u );
				m_terrainRockMaterial.apply( pContext, 11u );
			}

			pContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST );

			const XMMATRIX modelMatrix = XMMatrixIdentity();
			renderModel( pContext, m_terrainModel, modelMatrix );

			pContext->HSSetShader( nullptr, nullptr, 0u );
			pContext->DSSetShader( nullptr, nullptr, 0u );

			pContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
		}
	}

	void Sample05Tessellation::renderModel( ID3D11DeviceContext* pContext, const SampleModel& model, const XMMATRIX& transform )
	{
		{
			VertexModelConstantData* pConstantData = m_vertexModelConstants.map< VertexModelConstantData >( pContext );
			XMStoreFloat4x4( &pConstantData->world, XMMatrixTranspose( transform ) );
			XMStoreFloat4x4( &pConstantData->normal, XMMatrixInverse( nullptr, transform ) );
			m_vertexModelConstants.unmap( pContext );
		}

		model.render( pContext );
	}
}

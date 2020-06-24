#include "Sample04Effects.hpp"

#include "Sample04Types.hpp"

#include "../Shader/pbr_shader_types.hpp"

#include <d3d11.h>
#include <math.h>

namespace GamesAcademy
{
	static const char* WindowTitle = u8"CS310 04-Effects 🍩";
	static const int WindowWidth = 1280;
	static const int WindowHeight = 720;

	int Sample04Effects::run()
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

	bool Sample04Effects::create()
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

	void Sample04Effects::destroy()
	{
		destroyResources();
		m_timer.destroy();
		m_input.destroy();
		m_filesystem.destroy();
		m_states.destroy();
		m_device.destroy();
		m_window.destroy();
	}

	bool Sample04Effects::createResources()
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

			if( !m_pbrVertexShader.createVertexShaderFromFile(m_device, m_filesystem, "Shader/pbr.vs.hlsl", inputElements, ARRAY_COUNT( inputElements ) ) )
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

		if( !m_pbrPixelShader.createPixelShaderFromFile( m_device, m_filesystem, "Shader/pbr.ps.hlsl" ) ||
			!m_skyboxPixelShader.createPixelShaderFromFile( m_device, m_filesystem, "Shader/skybox.ps.hlsl" ) )
		{
			return false;
		}

		if( !m_vertexSceneConstants.create( m_device, sizeof( Sample04VertexSceneConstantData ) ) ||
			!m_vertexModelConstants.create( m_device, sizeof( Sample04VertexModelConstantData ) ) ||
			!m_pixelLightConstants.create( m_device, sizeof( PixelLightConstants ) ) ||
			!m_pixelMaterialConstants.create( m_device, sizeof( PbrPixelMaterialConstants ) ) )
		{
			return false;
		}

		if( !m_shadowMap.createEmptyShadowMap( m_device, 2048u, 2048u ) ||
			!m_wallModel.createFromFile( m_device, m_filesystem, "Content/ancient_wall/ancient_wall.obj" ) ||
			!m_pillar1Model.createFromFile( m_device, m_filesystem, "Content/ancient_wall/ancient_wall_pillar1.obj" ) ||
			!m_pillar2Model.createFromFile( m_device, m_filesystem, "Content/ancient_wall/ancient_wall_pillar2.obj" ) ||
			!m_wallPillarMaterial.createFromFiles( m_device, m_filesystem, "Content/ancient_wall/ancient_wall" ) ||
			!m_barrelModel.createFromFile( m_device, m_filesystem, "Content/barrel/barrel.obj", true ) ||
			!m_barrelMaterial.createFromFiles( m_device, m_filesystem, "Content/barrel/barrel" ) ||
			!m_stoneModel.createFromFile( m_device, m_filesystem, "Content/stone/stone.obj" ) ||
			!m_stoneMaterial.createFromFiles( m_device, m_filesystem, "Content/stone/stone" ) ||
			!m_planeModel.createFromFile( m_device, m_filesystem, "Content/plane.obj", true ) ||
			!m_planeMaterial.createFromFiles( m_device, m_filesystem, "Content/terrain/grass" ) ||
			!m_sphereModel.createFromFile( m_device, m_filesystem, "Content/sphere.obj" ) ||
			!m_skyboxModel.createFromFile( m_device, m_filesystem, "Content/skybox/skybox.obj" ) ||
			!m_environmentMap.createFromFile( m_device, m_filesystem, "Content/skybox/skybox_sea.dds" ) )
		{
			return false;
		}

		{
			const XMVECTOR cameraPosition		= XMVectorSet( -8.81523037f, 5.90831995f, 13.0256882f, 0.0f );
			const XMFLOAT2 cameraOrientation	= { 0.305851579f, 2.58151031f };
			m_camera.create( m_input, true, cameraPosition, cameraOrientation, 10.0f );
		}

		return true;
	}

	void Sample04Effects::destroyResources()
	{
		m_camera.destroy();

		m_skyboxModel.destroy();
		m_environmentMap.destroy();

		m_sphereModel.destroy();

		m_wallPillarMaterial.destroy();
		m_pillar2Model.destroy();
		m_pillar1Model.destroy();
		m_wallModel.destroy();

		m_stoneMaterial.destroy();
		m_stoneModel.destroy();

		m_planeMaterial.destroy();
		m_planeModel.destroy();

		m_barrelMaterial.destroy();
		m_barrelModel.destroy();

		m_shadowMap.destroy();

		m_pixelMaterialConstants.destroy();
		m_pixelLightConstants.destroy();
		m_vertexModelConstants.destroy();
		m_vertexSceneConstants.destroy();

		m_pbrPixelShader.destroy();
		m_pbrVertexShader.destroy();
		m_skyboxPixelShader.destroy();
		m_skyboxVertexShader.destroy();
	}

	void Sample04Effects::update()
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
			m_pbrTest = !m_pbrTest;
		}

		if( m_input.isMouseButtonDown( SampleInputMouseButton::Left ) )
		{
			m_lightDirection = -XMVector3Normalize( m_camera.getForward() );
		}
	}

	void Sample04Effects::render()
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

		pContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

		m_vertexSceneConstants.applyToVertexShader( pContext, 0u );
		m_vertexModelConstants.applyToVertexShader( pContext, 1u );

		m_pixelLightConstants.applyToPixelShader( pContext, 0u );

		m_states.applySamplerStateToPixelShader( pContext, 0u, SampleSamplerFilter::Anisotropic );
		m_states.applySamplerStateToPixelShader( pContext, 1u, SampleSamplerFilter::Point, SampleSamplerAddressMode::Clamp );

		XMMATRIX shadowMatrix;
		{
			m_shadowMap.applyRenderTargetAndClear( pContext );

			m_pbrVertexShader.apply( pContext, PBR_MODE_SHADOW );
			m_pbrPixelShader.apply( pContext, PBR_MODE_SHADOW );

			const XMVECTOR cameraPosition	= XMVectorScale( m_lightDirection, 10.0f );
			const XMMATRIX view				= XMMatrixLookToLH( cameraPosition, -m_lightDirection, XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0 ) );
			const XMMATRIX projection		= XMMatrixOrthographicOffCenterLH( -10.0f, 10.0f, -10.0, 10.0f, 1.0f, 50.0f );

			shadowMatrix					= view * projection;

			renderScene( pContext, view, projection, XMMatrixIdentity(), true );
		}

		{
			m_device.applyBackBuffer( pContext );

			m_pbrVertexShader.apply( pContext, PBR_MODE_DEFAULT );

			m_shadowMap.applyToPixelShader( pContext, 0u );

			const XMMATRIX view				= m_camera.getViewMatrix();
			const XMMATRIX projection		= XMMatrixPerspectiveFovLH( XM_PI / 4.0f, float( m_window.getClientWidth() ) / float( m_window.getClientHeight() ), 0.1f, 100.0f );

			renderScene( pContext, view, projection, shadowMatrix, false );

			// Clear Shadow Map slot
			ID3D11ShaderResourceView* pNullResource = nullptr;
			pContext->PSSetShaderResources( 0u, 1u, &pNullResource );
		}

		{
			m_skyboxVertexShader.apply( pContext );

			m_environmentMap.applyToPixelShader( pContext, 0u );
			m_skyboxPixelShader.apply( pContext );

			const XMMATRIX modelMatrix = XMMatrixScaling( 50.0f, 50.0f, 50.0f );
			renderModel(pContext, m_skyboxModel, modelMatrix );
		}

		m_device.endFrame();
	}

	void Sample04Effects::renderScene( ID3D11DeviceContext* pContext, const XMMATRIX& view, const XMMATRIX& projection, const XMMATRIX& shadowMatrix, bool shadowMap )
	{
		{
			Sample04VertexSceneConstantData* pConstantData = m_vertexSceneConstants.map< Sample04VertexSceneConstantData >( pContext );
			XMStoreFloat4x4( &pConstantData->viewProjection, XMMatrixTranspose( view * projection ) );
			XMStoreFloat4x4( &pConstantData->shadowViewProjection, XMMatrixTranspose( shadowMatrix ) );
			m_vertexSceneConstants.unmap( pContext );
		}

		if( m_pbrTest )
		{
			if( !shadowMap )
			{
				m_pbrPixelShader.apply( pContext, PBR_PS_MODE_CONSTANT );
			}

			m_pixelMaterialConstants.applyToPixelShader( pContext, 1u );

			XMMATRIX modelMatrix;
			for( int x = -3; x < 4; ++x )
			{
				const float roughness = max( 0.05f, (x + 3) / 6.0f );

				for( int z = -3; z < 4; ++z )
				{
					const float metallic = (z + 3) / 6.0f;

					{
						PbrPixelMaterialConstants* pConstantData = m_pixelMaterialConstants.map< PbrPixelMaterialConstants >( pContext );
						fillPbrPixelMaterialConstants( *pConstantData, { 1.0f, 0.0f, 0.0f }, 1.0f, metallic, roughness );
						m_pixelMaterialConstants.unmap( pContext );
					}

					modelMatrix = XMMatrixScaling( 0.5f, 0.5f, 0.5f ) * XMMatrixTranslation( float( x * -1.5f ), 0.75f, float( z * -1.5f ) );
					renderModel( pContext, m_sphereModel, modelMatrix );
				}
			}
		}

		if( !shadowMap )
		{
			m_pbrPixelShader.apply( pContext, PBR_MODE_DEFAULT );
		}

		if( !m_pbrTest )
		{
			if( !shadowMap )
			{
				m_wallPillarMaterial.apply( pContext, 1u );
			}

			XMMATRIX modelMatrix = XMMatrixTranslation( 0.0f, -0.25f, 0.0f );
			renderModel( pContext, m_wallModel, modelMatrix );

			modelMatrix = XMMatrixRotationX( XM_PI / -2.0f ) * XMMatrixTranslation( -2.0f, 0.25f, 0.0f );
			renderModel( pContext, m_pillar1Model, modelMatrix );

			modelMatrix = XMMatrixRotationY( XM_PI ) * XMMatrixRotationZ( XM_PI / -8.0f ) * XMMatrixTranslation( -7.25f, -0.25f, 0.0f );
			renderModel( pContext, m_pillar2Model, modelMatrix );

			if( !shadowMap )
			{
				m_stoneMaterial.apply( pContext, 1u );
			}

			modelMatrix = XMMatrixRotationY( XM_PI / 8.0f ) * XMMatrixTranslation( 3.0f, -0.1f, -5.0f );
			renderModel( pContext, m_stoneModel, modelMatrix );

			modelMatrix = XMMatrixRotationZ( XM_PI * -0.66f ) * XMMatrixTranslation( -4.0f, 0.5f, 6.0f );
			renderModel( pContext, m_stoneModel, modelMatrix );

			if( !shadowMap )
			{
				m_barrelMaterial.apply( pContext, 1u );
			}

			modelMatrix = XMMatrixTranslation( 0.8f, 0.0f, 0.7f );
			renderModel( pContext, m_barrelModel, modelMatrix );
		}

		{
			if( !shadowMap )
			{
				m_planeMaterial.apply( pContext, 1u );
			}

			const XMMATRIX modelMatrix = XMMatrixScaling( 2.0f, 2.0f, 2.0f );
			renderModel( pContext, m_planeModel, modelMatrix );
		}
	}

	void Sample04Effects::renderModel( ID3D11DeviceContext* pContext, const SampleModel& model, const XMMATRIX& transform )
	{
		{
			Sample04VertexModelConstantData* pConstantData = m_vertexModelConstants.map< Sample04VertexModelConstantData >( pContext );
			XMStoreFloat4x4( &pConstantData->world, XMMatrixTranspose( transform ) );
			XMStoreFloat4x4( &pConstantData->normal, XMMatrixInverse( nullptr, transform ) );
			m_vertexModelConstants.unmap( pContext );
		}

		model.render( pContext );
	}
}

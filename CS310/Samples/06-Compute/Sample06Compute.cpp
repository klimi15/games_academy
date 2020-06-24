#include "Sample06Compute.hpp"

#include "Sample06Types.hpp"

#include "../Shader/particle_shader_types.hpp"
#include "../Shader/pbr_shader_types.hpp"

#include <d3d11.h>
#include <math.h>

namespace GamesAcademy
{
	static const char* WindowTitle = u8"CS310 06-Compute 🍩";
	static const int WindowWidth = 1280;
	static const int WindowHeight = 720;

	int Sample06Compute::run()
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

	bool Sample06Compute::create()
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

	void Sample06Compute::destroy()
	{
		destroyResources();
		m_timer.destroy();
		m_input.destroy();
		m_filesystem.destroy();
		m_states.destroy();
		m_device.destroy();
		m_window.destroy();
	}

	bool Sample06Compute::createResources()
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

			if( !m_pbrVertexShader.createVertexShaderFromFile( m_device, m_filesystem, "Shader/pbr.vs.hlsl", inputElements, ARRAY_COUNT( inputElements ) ) )
			{
				return false;
			}
		}

		{
			const D3D11_INPUT_ELEMENT_DESC inputElements[] =
			{
				{ "POSITION",	0u, DXGI_FORMAT_R32G32B32_FLOAT,	0u, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0u }
			};

			if( !m_particleVertexShader.createVertexShaderFromFile( m_device, m_filesystem, "Shader/particle.vs.hlsl", inputElements, ARRAY_COUNT( inputElements ) ) )
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

		if( !m_particleComputeShader.createComputeShaderFromFile( m_device, m_filesystem, "Shader/particle.cs.hlsl" ) ||
			!m_particleGeometryShader.createGeometryShaderFromFile( m_device, m_filesystem, "Shader/particle.gs.hlsl" ) ||
			!m_pbrPixelShader.createPixelShaderFromFile( m_device, m_filesystem, "Shader/pbr.ps.hlsl" ) ||
			!m_particlePixelShader.createPixelShaderFromFile( m_device, m_filesystem, "Shader/particle.ps.hlsl" ) ||
			!m_skyboxPixelShader.createPixelShaderFromFile( m_device, m_filesystem, "Shader/skybox.ps.hlsl" ) )
		{
			return false;
		}

		if( !m_simulationBuffer.create( m_device, sizeof( ParticleSimulationState ), ParticleCount ) ||
			!m_renderBuffer.create( m_device, sizeof( ParticleRenderState ), ParticleCount ) )
		{
			return false;
		}

		if( !m_computeConstants.create( m_device, sizeof( ParticleComputeConstantData ) ) ||
			!m_vertexViewConstants.create( m_device, sizeof( VertexViewConstantData ) ) ||
			!m_vertexModelConstants.create( m_device, sizeof( VertexModelConstantData ) ) ||
			!m_geometryViewConstants.create( m_device, sizeof( ParticleGeometryViewConstantData ) ) ||
			!m_pixelLightConstants.create( m_device, sizeof( PixelLightConstants ) ) )
		{
			return false;
		}

		if( !m_dotTexture.createFromFile( m_device, m_filesystem, "Content/particle/dot.dds" ) ||
			!m_shadowMap.createEmptyShadowMap( m_device, 2048u, 2048u ) ||
			!m_sphereModel.createFromFile( m_device, m_filesystem, "Content/sphere.obj" ) ||
			!m_sphereMaterial.createFromFiles( m_device, m_filesystem, "Content/particle/sun" ) ||
			!m_skyboxModel.createFromFile( m_device, m_filesystem, "Content/skybox/skybox.obj" ) ||
			!m_environmentMap.createFromFile( m_device, m_filesystem, "Content/skybox/skybox_space.dds" ) )
		{
			return false;
		}

		{
			const XMVECTOR cameraPosition		= XMVectorSet( -1.739075f, 1.95422f, 0.176270f, 0.0f );
			const XMFLOAT2 cameraOrientation	= { 0.115978763f, 0.0f };
			m_camera.create( m_input, true, cameraPosition, cameraOrientation, 10.0f );
		}

		return true;
	}

	void Sample06Compute::destroyResources()
	{
		m_camera.destroy();

		m_skyboxModel.destroy();
		m_environmentMap.destroy();

		m_sphereMaterial.destroy();
		m_sphereModel.destroy();

		m_shadowMap.destroy();

		m_pixelLightConstants.destroy();
		m_geometryViewConstants.destroy();
		m_vertexModelConstants.destroy();
		m_vertexViewConstants.destroy();
		m_computeConstants.destroy();

		m_renderBuffer.destroy();
		m_simulationBuffer.destroy();

		m_skyboxPixelShader.destroy();
		m_skyboxVertexShader.destroy();
		m_particlePixelShader.destroy();
		m_particleGeometryShader.destroy();
		m_particleVertexShader.destroy();
		m_particleComputeShader.destroy();
		m_pbrPixelShader.destroy();
		m_pbrVertexShader.destroy();
	}

	void Sample06Compute::update()
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

		//if( m_input.wasKeyboardKeyPressed( SampleInputKeyboardKey::Space ) )
		//{
		//	m_wireframe = !m_wireframe;
		//}

		//if (m_input.wasKeyboardKeyPressed( SampleInputKeyboardKey::F1 ))
		//{
		//	m_tessellationFactor = clamp( 0.0f, 5.0f, m_tessellationFactor - 0.05f );
		//}
		//else if( m_input.wasKeyboardKeyPressed( SampleInputKeyboardKey::F2 ) )
		//{
		//	m_tessellationFactor = clamp( 0.0f, 5.0f, m_tessellationFactor + 0.05f );
		//}

		if( m_input.isMouseButtonDown( SampleInputMouseButton::Left ) )
		{
			m_lightDirection = -XMVector3Normalize( m_camera.getForward() );
		}
	}

	void Sample06Compute::render()
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

		m_vertexViewConstants.applyToVertexShader( pContext, 0u );
		m_vertexModelConstants.applyToVertexShader( pContext, 1u );

		m_pixelLightConstants.applyToPixelShader( pContext, 0u );

		m_states.applySamplerStateToPixelShader( pContext, 0u, SampleSamplerFilter::Anisotropic );
		m_states.applySamplerStateToPixelShader( pContext, 1u, SampleSamplerFilter::Point, SampleSamplerAddressMode::Clamp );
		m_states.applySamplerStateToPixelShader( pContext, 2u, SampleSamplerFilter::Linear, SampleSamplerAddressMode::Clamp );

		{
			{
				ParticleComputeConstantData* pConstantData = m_computeConstants.map< ParticleComputeConstantData >( pContext );
				fillParticleComputeConstantData( pConstantData, float( ParticleCount ), float( m_timer.getGameTime() ), float( m_timer.getDeltaTime() ) );
				m_computeConstants.unmap( pContext );
			}

			m_computeConstants.applyToComputeShader( pContext, 0u );

			m_particleComputeShader.apply( pContext );

			m_simulationBuffer.applyUnorderedAccess( pContext, 0u );
			m_renderBuffer.applyUnorderedAccess( pContext, 1u );

			pContext->Dispatch( ParticleCount, 1u, 1u );

			UINT initialCount[] = { UINT( -1 ), UINT( -1 ) };
			ID3D11UnorderedAccessView* pUnorderedAccessViews[] = { nullptr, nullptr };
			pContext->CSSetUnorderedAccessViews( 0u, 2u, pUnorderedAccessViews, initialCount );
		}

		XMMATRIX shadowMatrix;
		{
			m_shadowMap.applyRenderTargetAndClear( pContext );

			const XMVECTOR cameraPosition	= XMVectorScale( m_lightDirection, -10.0f );
			const XMMATRIX view				= XMMatrixLookToLH( cameraPosition, -m_lightDirection, XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0 ) );
			const XMMATRIX projection		= XMMatrixOrthographicOffCenterLH( -10.0f, 10.0f, -10.0, 10.0f, 1.0f, 50.0f );

			shadowMatrix					= view * projection;

			//renderScene( pContext, view, projection, XMMatrixIdentity(), true );
		}

		m_device.applyBackBuffer( pContext );

		{
			m_skyboxVertexShader.apply( pContext );

			m_environmentMap.applyToPixelShader( pContext, 0u );
			m_skyboxPixelShader.apply( pContext );

			const XMMATRIX modelMatrix = XMMatrixScaling( 50.0f, 50.0f, 50.0f );
			renderModel( pContext, m_skyboxModel, modelMatrix );
		}

		{
			m_shadowMap.applyToPixelShader( pContext, 0u );

			const XMMATRIX view				= m_camera.getViewMatrix();
			const XMMATRIX projection		= XMMatrixPerspectiveFovLH( XM_PI / 4.0f, float( m_window.getClientWidth() ) / float( m_window.getClientHeight() ), 0.1f, 100.0f );

			renderScene( pContext, view, projection, shadowMatrix, false );

			// Clear Shadow Map slot
			ID3D11ShaderResourceView* pNullResource = nullptr;
			pContext->PSSetShaderResources( 0u, 1u, &pNullResource );
		}

		m_device.endFrame();
	}

	void Sample06Compute::renderScene( ID3D11DeviceContext* pContext, const XMMATRIX& view, const XMMATRIX& projection, const XMMATRIX& shadowMatrix, bool shadowMap )
	{
		{
			VertexViewConstantData* pConstantData = m_vertexViewConstants.map< VertexViewConstantData >( pContext );
			XMStoreFloat4x4( &pConstantData->viewProjection, XMMatrixTranspose( view * projection ) );
			XMStoreFloat4x4( &pConstantData->shadowViewProjection, XMMatrixTranspose( shadowMatrix ) );
			m_vertexViewConstants.unmap( pContext );
		}

		{
			const size_t shaderMode = (shadowMap ? PBR_MODE_SHADOW : PBR_MODE_DEFAULT);
			m_pbrVertexShader.apply( pContext, shaderMode );
			m_pbrPixelShader.apply( pContext, shaderMode );

			if( !shadowMap )
			{
				m_sphereMaterial.apply( pContext, 1u );
			}

			const XMMATRIX modelMatrix = XMMatrixScaling( 2.0f, 2.0f, 2.0 );
			renderModel( pContext, m_sphereModel, modelMatrix );
		}

		if( !shadowMap )
		{
			{
				VertexModelConstantData* pConstantData = m_vertexModelConstants.map< VertexModelConstantData >( pContext );
				XMStoreFloat4x4( &pConstantData->world, XMMatrixIdentity() );
				XMStoreFloat4x4( &pConstantData->normal, XMMatrixIdentity() );
				m_vertexModelConstants.unmap( pContext );
			}

			{
				ParticleGeometryViewConstantData* pConstantData = m_geometryViewConstants.map< ParticleGeometryViewConstantData >( pContext );
				XMStoreFloat4x4( &pConstantData->viewTranspose, view );
				XMStoreFloat4x4( &pConstantData->viewProjection, XMMatrixTranspose( view * projection ) );
				m_geometryViewConstants.unmap( pContext );
			}

			pContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_POINTLIST );

			m_particleVertexShader.apply( pContext );
			m_particleGeometryShader.apply( pContext );
			m_particlePixelShader.apply( pContext );

			m_renderBuffer.applyResourceToVertexShader( pContext, 0u );

			m_geometryViewConstants.applyToGeometryShader( pContext, 0u );

			m_dotTexture.applyToPixelShader( pContext, 0u );

			m_states.applyDepthState( pContext, false );
			m_states.applyBlendState( pContext, true );

			UINT vertexStride = 0u;
			UINT vertexOffset = 0u;
			ID3D11Buffer* pVertexBuffer = nullptr;
			pContext->IASetVertexBuffers( 0u, 1u, &pVertexBuffer, &vertexStride, &vertexOffset );
			pContext->IASetVertexBuffers( 0u, 1u, &pVertexBuffer, &vertexStride, &vertexOffset );

			pContext->IASetIndexBuffer( nullptr, DXGI_FORMAT_UNKNOWN, 0u );

			pContext->Draw( ParticleCount, 0u );

			ID3D11ShaderResourceView* pResourceView = nullptr;
			pContext->VSSetShaderResources( 0u, 1u, &pResourceView );

			pContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
			pContext->GSSetShader( nullptr, nullptr, 0u );

			m_states.applyDepthState( pContext, true );
			m_states.applyBlendState( pContext, false );
		}
	}

	void Sample06Compute::renderModel( ID3D11DeviceContext* pContext, const SampleModel& model, const XMMATRIX& transform )
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

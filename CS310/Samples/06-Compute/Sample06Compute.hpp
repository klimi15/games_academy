#pragma once

#include "../01-Window/SampleDevice.hpp"
#include "../01-Window/SampleWindow.hpp"
#include "../02-Triangle/SampleGraphicsStates.hpp"
#include "../02-Triangle/SampleTimer.hpp"
#include "../03-Box/SampleCamera.hpp"
#include "../03-Box/SampleFilesystem.hpp"
#include "../03-Box/SampleInput.hpp"
#include "../03-Box/SampleShaderConstants.hpp"
#include "../03-Box/SampleTexture.hpp"
#include "../04-Effects/SampleMaterial.hpp"
#include "../04-Effects/SampleModel.hpp"
#include "../04-Effects/SampleShaderVariants.hpp"

#include "SampleGraphicsBuffer.hpp"

struct ID3D11DeviceContext;

namespace GamesAcademy
{
	class Sample06Compute
	{
	public:

		int							run();

	private:

		static const size_t			ParticleCount					= 32u * 1024u;

		bool						m_running						= false;

		SampleWindow				m_window;
		SampleDevice				m_device;
		SampleGraphicsStates		m_states;
		SampleFilesystem			m_filesystem;
		SampleInput					m_input;
		SampleTimer					m_timer;
		SampleCamera				m_camera;

		SampleShaderVariants		m_pbrVertexShader;
		SampleShaderVariants		m_pbrPixelShader;
		SampleShaderVariants		m_particleComputeShader;
		SampleShaderVariants		m_particleVertexShader;
		SampleShaderVariants		m_particleGeometryShader;
		SampleShaderVariants		m_particlePixelShader;
		SampleShaderVariants		m_skyboxVertexShader;
		SampleShaderVariants		m_skyboxPixelShader;

		SampleGraphicsBuffer		m_simulationBuffer;
		SampleGraphicsBuffer		m_renderBuffer;

		SampleTexture				m_dotTexture;
		//SampleTexture				m_shadowMap;

		SampleTexture				m_shadowMap;

		SampleModel					m_sphereModel;
		SampleMaterial				m_sphereMaterial;

		SampleTexture				m_environmentMap;
		SampleModel					m_skyboxModel;

		SampleShaderConstants		m_computeConstants;
		SampleShaderConstants		m_vertexViewConstants;
		SampleShaderConstants		m_vertexModelConstants;
		SampleShaderConstants		m_geometryViewConstants;
		SampleShaderConstants		m_pixelLightConstants;

		XMVECTOR					m_lightDirection				= XMVectorZero();
		float						m_backgroundColor[ 4u ]			= { 0.003922f, 0.396078f, 0.988235f, 1.0f };

		bool						create();
		void						destroy();

		bool						createResources();
		void						destroyResources();

		void						update();

		void						render();
		void						renderScene( ID3D11DeviceContext* pContext, const XMMATRIX& view, const XMMATRIX& projection, const XMMATRIX& shadowMatrix, bool shadowMap );
		void						renderModel( ID3D11DeviceContext* pContext, const SampleModel& model, const XMMATRIX& transform );
	};
}
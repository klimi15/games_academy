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

struct ID3D11DeviceContext;
struct ID3D11SamplerState;

namespace GamesAcademy
{
	class Sample05Tessellation
	{
	public:

		int							run();

	private:

		bool						m_running						= false;

		SampleWindow				m_window;
		SampleDevice				m_device;
		SampleGraphicsStates		m_states;
		SampleFilesystem			m_filesystem;
		SampleInput					m_input;
		SampleTimer					m_timer;
		SampleCamera				m_camera;

		SampleShaderVariants		m_terrainVertexShader;
		SampleShaderVariants		m_terrainHullShader;
		SampleShaderVariants		m_terrainDomainShader;
		SampleShaderVariants		m_terrainPixelShader;
		SampleShaderVariants		m_skyboxVertexShader;
		SampleShaderVariants		m_skyboxPixelShader;

		SampleTexture				m_shadowMap;

		SampleModel					m_terrainModel;
		SampleMaterial				m_terrainSandMaterial;
		SampleMaterial				m_terrainGrassMaterial;
		SampleMaterial				m_terrainRockMaterial;
		SampleTexture				m_terrainHeightMap;
		SampleTexture				m_terrainBlendMap;

		SampleTexture				m_environmentMap;
		SampleModel					m_skyboxModel;

		SampleShaderConstants		m_vertexViewConstants;
		SampleShaderConstants		m_vertexModelConstants;
		SampleShaderConstants		m_hullViewConstants;
		SampleShaderConstants		m_pixelLightConstants;

		bool						m_wireframe						= false;
		float						m_tessellationFactor			= 1.0f;
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
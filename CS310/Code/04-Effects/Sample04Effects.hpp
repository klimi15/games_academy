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

#include "SampleMaterial.hpp"
#include "SampleModel.hpp"
#include "SampleShaderVariants.hpp"

#include "Sample04Types.hpp"

struct ID3D11DeviceContext;

namespace GamesAcademy
{
	class Sample04Effects
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

		SampleShaderVariants		m_pbrVertexShader;
		SampleShaderVariants		m_pbrPixelShader;
		SampleShaderVariants		m_skyboxVertexShader;
		SampleShaderVariants		m_skyboxPixelShader;

		SampleTexture				m_shadowMap;

		SampleModel					m_wallModel;
		SampleModel					m_pillar1Model;
		SampleModel					m_pillar2Model;
		SampleMaterial				m_wallPillarMaterial;

		SampleModel					m_barrelModel;
		SampleMaterial				m_barrelMaterial;

		SampleModel					m_stoneModel;
		SampleMaterial				m_stoneMaterial;

		SampleModel					m_planeModel;
		SampleMaterial				m_planeMaterial;

		SampleModel					m_sphereModel;

		SampleTexture				m_environmentMap;
		SampleModel					m_skyboxModel;

		SampleShaderConstants		m_vertexSceneConstants;
		SampleShaderConstants		m_vertexModelConstants;
		SampleShaderConstants		m_pixelLightConstants;
		SampleShaderConstants		m_pixelMaterialConstants;

		bool						m_pbrTest						= false;
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
#pragma once

#include "../01-Window/SampleDevice.hpp"
#include "../01-Window/SampleWindow.hpp"
#include "../02-Triangle/SampleGraphicsStates.hpp"
#include "../02-Triangle/SampleShader.hpp"
#include "../02-Triangle/SampleTimer.hpp"

#include "SampleCamera.hpp"
#include "SampleFilesystem.hpp"
#include "SampleInput.hpp"
#include "SampleShaderConstants.hpp"
#include "SampleTexture.hpp"

struct ID3D11Buffer;

namespace GamesAcademy
{
	class Sample03Box
	{
	public:

		int						run();

	private:

		bool					m_running			= false;

		SampleWindow			m_window;
		SampleDevice			m_device;
		SampleGraphicsStates	m_states;
		SampleFilesystem		m_filesystem;
		SampleInput				m_input;
		SampleTimer				m_timer;
		SampleCamera			m_camera;

		SampleShader			m_vertexShader;
		SampleShader			m_pixelShader;

		SampleTexture			m_texture;

		SampleShaderConstants	m_vertexConstants;

		ID3D11Buffer*			m_pVertexBuffer		= nullptr;
		ID3D11Buffer*			m_pIndexBuffer		= nullptr;
		uint32					m_indexCount		= 0u;

		float					m_backgroundColor[ 4u ] = { 0.003922f, 0.396078f, 0.988235f, 1.0f };

		bool					create();
		void					destroy();

		bool					createResources();
		void					destroyResources();

		void					update();
		void					render();
	};
}
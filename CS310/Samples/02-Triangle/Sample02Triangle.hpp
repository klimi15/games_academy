#pragma once

#include "../01-Window/SampleDevice.hpp"
#include "../01-Window/SampleWindow.hpp"

#include "SampleGraphicsStates.hpp"
#include "SampleShader.hpp"
#include "SampleTimer.hpp"

struct ID3D11Buffer;

namespace GamesAcademy
{
	class Sample02Triangle
	{
	public:

		int						run();

	private:

		SampleWindow			m_window;
		SampleDevice			m_device;
		SampleGraphicsStates	m_states;
		SampleTimer				m_timer;

		SampleShader			m_vertexShader;
		SampleShader			m_pixelShader;

		ID3D11Buffer*			m_pVertexBuffer			= nullptr;

		float					m_backgroundColor[ 4u ]	= { 0.003922f, 0.396078f, 0.988235f, 1.0f };

		bool					create();
		void					destroy();

		bool					createResources();
		void					destroyResources();

		void					update();
		void					render();
	};
}
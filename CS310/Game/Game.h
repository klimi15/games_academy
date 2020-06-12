#pragma once

#include "Graphics.h"
#include "Input.h"
#include "Model.h"

namespace GA
{
	class Game
	{
	public:

		int						run();

	private:

		Graphics				m_graphics;
		Input					m_input;

		bool					m_running;

		GraphicsShaderHandle	m_vertexShader;
		GraphicsShaderHandle	m_pixelShader;

		Model					m_plane;

		void					update();
		void					render();
	};
}

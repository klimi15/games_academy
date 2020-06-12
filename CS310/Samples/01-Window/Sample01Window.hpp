#pragma once

#include "SampleWindow.hpp"
#include "SampleDevice.hpp"

namespace GamesAcademy
{
	class Sample01Window
	{
	public:

		int						run();

	private:

		SampleWindow			m_window;
		SampleDevice			m_device;

		float					m_backgroundColor[ 4u ]	= { 0.003922f, 0.396078f, 0.988235f, 1.0f };

		void					update();
		void					render();
	};
}
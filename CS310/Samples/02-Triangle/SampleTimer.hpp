#pragma once

#include "../01-Window/SampleTypes.hpp"

namespace GamesAcademy
{
	class SampleTimer
	{
	public:

					SampleTimer();
					~SampleTimer();

		void		create();
		void		destroy();

		void		update();

		double		getGameTime() const { return m_gameTime; }
		double		getDeltaTime() const { return m_deltaTime; }

	private:

		uint64		m_frequency = 0u;
		uint64		m_startTime = 0u;

		double		m_gameTime	= 0.0;
		double		m_deltaTime	= 0.0;
	};
}

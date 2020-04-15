#pragma once

#include <windows.h>

namespace GamesAcademy
{
	class Timer
	{
	public:

		void			create();

		double			get() const;

	private:

		LARGE_INTEGER	m_timeFrequency;
		LARGE_INTEGER	m_startTime;
	};
}
#include "SampleTimer.hpp"

#include <Windows.h>

namespace GamesAcademy
{
	SampleTimer::SampleTimer()
	{
		static_assert( sizeof( m_frequency ) == sizeof( LARGE_INTEGER ), "Invalid Size" );
		static_assert( sizeof( m_startTime ) == sizeof( LARGE_INTEGER ), "Invalid Size" );
	}

	SampleTimer::~SampleTimer()
	{
		destroy();
	}

	void SampleTimer::create()
	{
		QueryPerformanceFrequency( (LARGE_INTEGER*)&m_frequency );
		QueryPerformanceCounter( (LARGE_INTEGER*)&m_startTime );
	}

	void SampleTimer::destroy()
	{
		m_startTime	= 0u;
		m_frequency	= 0u;
		m_gameTime	= 0.0;
		m_deltaTime	= 0.0;
	}

	void SampleTimer::update()
	{
		LARGE_INTEGER currentTime;
		QueryPerformanceCounter( &currentTime );

		const double gameTime = double( currentTime.QuadPart - m_startTime ) / double( m_frequency );
		m_deltaTime = gameTime - m_gameTime;
		m_gameTime	= gameTime;
	}
}

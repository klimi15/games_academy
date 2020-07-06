#include "threading.h"

#include <iostream>
#include <stdio.h>
#include <assert.h>

namespace GamesAcademy
{
	int Threading::run()
	{
		std::cout << "Main Thread started." << std::endl;

		m_numbersEvent = CreateEvent( nullptr, FALSE, FALSE, L"Numbers" );

		SYSTEM_INFO info;
		GetSystemInfo( &info );
		const int workerCount = info.dwNumberOfProcessors - 1;

		std::cout << "Starting " << workerCount << " Threads." << std::endl;
		for( int i = 0u; i < workerCount; ++i )
		{
			m_threadHandle[ i ] = CreateThread( nullptr, 0, &staticThreadEntryPoint, this, 0, nullptr );
		}

		std::cout << "Main Thread init finish." << std::endl;

		m_top.store( 0 );
		m_bottom.store( 0 );

		long nextBottom = 0;
		while( true )
		{
			const int index = nextBottom & (2048 - 1);
			m_numbers[ index ] = rand();
			nextBottom++;

			assert( nextBottom != m_top.load( std::memory_order_relaxed ) );
			m_bottom.store( nextBottom, std::memory_order_release );

			SetEvent( m_numbersEvent );
		}

		return 0;
	}

	DWORD Threading::staticThreadEntryPoint( void* pArgument )
	{
		Threading* pThreading = (Threading*)pArgument;
		pThreading->threadEntryPoint();
		return 0;
	}

	void Threading::threadEntryPoint()
	{
		std::cout << "Thread started." << std::endl;

		while( true )
		{
			const int top = m_top.load( std::memory_order_acquire );
			if( m_bottom.load( std::memory_order_relaxed ) == top )
			{
				WaitForSingleObject( m_numbersEvent, INFINITE );
			}

			m_top++;
			const int index = top & (2048 - 1);
			int number = m_numbers[ index ];

			//std::cout << "Number is: " << number << std::endl;
		}
	}
}

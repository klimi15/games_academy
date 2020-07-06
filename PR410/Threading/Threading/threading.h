#pragma once

#include <Windows.h>

#include <vector>
#include <atomic>

namespace GamesAcademy
{
	class Threading
	{
	public:

		int					run();

	private:

		HANDLE				m_threadHandle[ 12u ];
		HANDLE				m_numbersEvent;

		int					m_numbers[ 2048 ];
		std::atomic< int >	m_top;
		std::atomic< int >	m_bottom;

		static DWORD WINAPI	staticThreadEntryPoint( void* pArgument );
		void				threadEntryPoint();
	};
}

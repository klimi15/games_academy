#pragma once

#include "Handle.h"

#include <stdint.h>

namespace GamesAcademy
{
	template< class T >
	class Pool
	{
	public:

		using THandle = Handle< T, uint32_t, 8u >;

					Pool();
					~Pool();

		THandle		newObject();
		void		deleteObject( THandle handle );

		T*			getObject( THandle handle );

	private:

		struct FreeElement
		{
			FreeElement*	pNext;
			uint32_t		index;
		};

		T*				m_pData;
		FreeElement*	m_pFirstFree;
		uint32_t		m_counter;
	};
}

#include "Pool.inl"
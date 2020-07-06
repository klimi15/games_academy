#include "Pool.h"

namespace GamesAcademy
{
	template< class T >
	Pool<T>::Pool()
	{
		m_pData = new T[ 64 ];
		m_counter = 0;

		for( uint32_t i = 0u; i < 64; ++i )
		{
			FreeElement* pFree = (FreeElement*)&m_pData[ i ];
			pFree->pNext = m_pFirstFree;
			pFree->index = i;

			m_pFirstFree = pFree;
		}
	}

	template< class T >
	Pool<T>::~Pool()
	{
		delete[] m_pData;
	}

	template< class T >
	typename Pool<T>::THandle Pool<T>::newObject()
	{
		if( m_pFirstFree == nullptr )
		{
			// out of memory
			return THandle::invalid();
		}

		THandle handle;
		handle.set( m_pFirstFree->index, m_counter );

		T* pObject = (T*)m_pFirstFree;
		pObject->handle = handle;

		m_pFirstFree = m_pFirstFree->pNext;

		return handle;
	}

	template< class T >
	void Pool<T>::deleteObject( THandle handle )
	{
		T* pObject = getObject( handle );
		if( pObject == nullptr )
		{
			return;
		}

		FreeElement* pFree = (FreeElement*)pObject;
		pFree->pNext = m_pFirstFree;
		pFree->index = handle.getIndex();

		m_counter++;
	}

	template< class T >
	T* Pool<T>::getObject( THandle handle )
	{
		if( handle == THandle::invalid() )
		{
			return nullptr;
		}

		const uint32_t index = handle.getIndex();
		T* pObject = &m_pData[ index ];
		if( pObject->handle != handle )
		{
			return nullptr;
		}

		return pObject;
	}
}

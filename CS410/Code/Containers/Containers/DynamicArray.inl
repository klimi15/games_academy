#pragma once

#include <assert.h>
#include <new>
#include <string.h>

namespace GamesAcademy
{
	template< class T >
	DynamicArray<T>::DynamicArray()
	{
	}

	template< class T >
	DynamicArray<T>::~DynamicArray()
	{
		destroy();
	}

	template< class T >
	void DynamicArray<T>::create( size_t initialCapacity /*= 0u */ )
	{
		reserve( initialCapacity );
	}

	template< class T >
	void DynamicArray<T>::destroy()
	{
		if( m_pData == nullptr )
		{
			return;
		}

		clear();

		free( m_pData );

		m_pData		= nullptr;
		m_capacity	= 0u;
		m_length	= 0u;
	}

	template< class T >
	bool DynamicArray<T>::isEmpty() const
	{
		return m_length == 0u;
	}

	template< class T >
	bool DynamicArray<T>::hasElements() const
	{
		return m_length != 0u;
	}

	template< class T >
	size_t DynamicArray<T>::getLength() const
	{
		return m_length;
	}

	template< class T >
	size_t DynamicArray<T>::getCapacity() const
	{
		return m_capacity;
	}

	template< class T >
	void DynamicArray<T>::clear()
	{
		resize( 0u );
	}

	template< class T >
	void DynamicArray<T>::resize( size_t length )
	{
		reserve( length );

		for( size_t i = length; i < m_length; ++i )
		{
			get( i )->~T();
		}

		for( size_t i = m_length; i < length; ++i )
		{
			T* pElement = get( i );
			new ( pElement ) T;
		}

		m_length = length;
	}

	template< class T >
	void DynamicArray<T>::reserve( size_t length )
	{
		if( length <= m_capacity )
		{
			return;
		}

		const size_t nextCapacity = m_capacity == 0u ? 1u : m_capacity << 1u;
		void* pData = malloc( nextCapacity * sizeof( T ) );
		memcpy( pData, m_pData, m_length * sizeof( T ) );

		free( m_pData );

		m_pData		= pData;
		m_capacity	= nextCapacity;
	}

	template< class T >
	T& DynamicArray<T>::pushBack()
	{
		resize( m_length + 1u );
		return *get( m_length - 1u );
	}

	template< class T >
	void DynamicArray<T>::pushBack( const T& value )
	{
		pushBack() = value;
	}

	template< class T >
	void DynamicArray<T>::popBack()
	{
		resize( m_length - 1u );
	}

	template< class T >
	void DynamicArray<T>::popBack( T& target )
	{
		target = *get( m_length - 1u );
		popBack();
	}

	template< class T >
	void DynamicArray<T>::removeSortedByIndex( size_t index )
	{
		get( index )->~T();

		for( size_t i = index + 1u; i < m_length; ++i )
		{
			memcpy( get( i - 1u ), get( i ), sizeof( T ) );
		}

		m_length--;
	}

	template< class T >
	void DynamicArray<T>::removeUnsortedByIndex( size_t index )
	{
		T* pElement = get( index );
		pElement->~T();
		memcpy( get( m_length - 1u ), pElement, sizeof( T ) );
		m_length--;
	}

	template< class T >
	T& DynamicArray<T>::operator[]( size_t index )
	{
		assert( index < m_length );
		return *get( index );
	}

	template< class T >
	const T& DynamicArray<T>::operator[]( size_t index ) const
	{
		assert( index < m_length );
		return *get( index );
	}

	template< class T >
	T* DynamicArray<T>::get( size_t index )
	{
		assert( index < m_capacity );
		T* pElements = (T*)m_pData;
		return &pElements[ index ];
	}

	template< class T >
	const T* DynamicArray<T>::get( size_t index ) const
	{
		return const_cast< DynamicArray< T >* >( this )->get( index );
	}
}

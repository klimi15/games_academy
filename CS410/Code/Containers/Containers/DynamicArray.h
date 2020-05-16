#pragma once

namespace GamesAcademy
{
	template< class T >
	class DynamicArray
	{
	public:

					DynamicArray();
					~DynamicArray();

		void		create( size_t initialCapacity = 0u );
		void		destroy();

		bool		isEmpty() const;
		bool		hasElements() const;
		size_t		getLength() const;
		size_t		getCapacity() const;

		void		clear();
		void		resize( size_t length );
		void		reserve( size_t length );

		T&			pushBack();
		void		pushBack( const T& value );
		T&			pushFront();
		void		pushFront( const T& value );

		void		popBack();
		void		popBack( T& target );
		void		popFront();
		void		popFront( T& target );

		void		removeSortedByIndex( size_t index );
		void		removeUnsortedByIndex( size_t index );

		T&			operator[]( size_t index );
		const T&	operator[]( size_t index ) const;

	private:

		void*		m_pData		= nullptr;
		size_t		m_capacity	= 0u;
		size_t		m_length	= 0u;

		T*			get( size_t index );
		const T*	get( size_t index ) const;
	};
}

#include "DynamicArray.inl"

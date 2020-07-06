#pragma once

namespace GamesAcademy
{
	template< class TTag, class TStorage, TStorage TIndexBits >
	class Handle
	{
	public:

		void			set( TStorage index, TStorage counter );

		TStorage		getIndex() const;

		bool			operator==( const Handle& rhs ) const;
		bool			operator!=( const Handle& rhs ) const;

		static Handle	invalid() { Handle h; h.m_value = 0; return h; }

	private:

		TStorage	m_value;
	};
}

#include "Handle.inl"
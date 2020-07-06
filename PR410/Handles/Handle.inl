#pragma once

namespace GamesAcademy
{
	template< class TTag, class TStorage, TStorage TIndexBits >
	void Handle<TTag, TStorage, TIndexBits>::set( TStorage index, TStorage counter )
	{
		m_value = index | ((counter + 1) << TIndexBits);
	}

	template< class TTag, class TStorage, TStorage TIndexBits >
	TStorage Handle<TTag, TStorage, TIndexBits>::getIndex() const
	{
		return m_value & ((1 << TIndexBits) - 1);
	}

	template< class TTag, class TStorage, TStorage TIndexBits >
	bool Handle<TTag, TStorage, TIndexBits>::operator==( const Handle& rhs ) const
	{
		return rhs.m_value == m_value;
	}

	template< class TTag, class TStorage, TStorage TIndexBits >
	bool Handle<TTag, TStorage, TIndexBits>::operator!=( const Handle& rhs ) const
	{
		return rhs.m_value != m_value;
	}
}
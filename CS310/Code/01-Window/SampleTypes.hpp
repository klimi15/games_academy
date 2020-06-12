#pragma once

#define ARRAY_COUNT( var ) (sizeof((var))/sizeof(*(var)))
#define ALIGN_VALUE( value, alignment ) (( value + alignment - 1 ) & ( 0 - alignment ))
#define SAFE_RELEASE( ptr ) do { if( ptr ) { ptr->Release(); ptr = nullptr; } } while( false )

namespace GamesAcademy
{
	using uint8 = unsigned __int8;
	using uint16 = unsigned __int16;
	using uint32 = unsigned __int32;
	using uint64 = unsigned __int64;

	using sint8 = __int8;
	using sint16 = __int16;
	using sint32 = __int32;
	using sint64 = __int64;

	struct MemoryBlock
	{
		const void* pData;
		size_t		size;
	};

	template< typename T >
	T clamp( T min, T max, T value )
	{
		return value < min ? min : (value > max ? max : value);
	}
}

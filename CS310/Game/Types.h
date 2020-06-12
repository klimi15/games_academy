#pragma once

#define ARRAY_COUNT( var ) (sizeof((var))/sizeof(*(var)))
#define ALIGN_VALUE( value, alignment ) (( value + alignment - 1 ) & ( 0 - alignment ))
#define SAFE_RELEASE( ptr ) do { if( ptr ) { ptr->Release(); ptr = nullptr; } } while( false )

namespace GA
{
	struct MemoryBlock
	{
		const void*	pData;
		size_t		size;
	};
}
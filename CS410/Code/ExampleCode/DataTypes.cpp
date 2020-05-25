#include <iostream>
#include <stdint.h>

struct A
{
	uint8_t		var1;	// min=0, max=255, size_bytes=1, alignment=1
	uint16_t	var2;	// min=0, max=65535, size_bytes=2, alignment=2
	uint32_t	var3;	// min=0, max=2^32-1, size_bytes=4, alignment=4
	uint64_t	var4;	// min=0, max=2^64-1, size_bytes=8, alignment=8

	int8_t		var5;	// min=-128, max=127, size_bytes=1, alignment=1
	int16_t		var6;	// min=-(2^16), max=(2^16)-1, size_bytes=2, alignment=2
};

struct B
{
}; // c_size=0, cpp_size=1

struct C
{
	uint8_t		var1;	// size=1, alignment=1
	// padding=1
	uint16_t	var2;	// size=2, alignment=2
}; // size=4, alignment=2

void runDataTypes()
{
	uint8_t data[] =
	{
		0x2A,
		0x00,
		0x39, 0x05
	};

	C structData;
	//structData = *(C*)data;
	memcpy( &structData, data, sizeof( structData ) );

	std::cout << "Var1: " << (uint32_t)structData.var1 << std::endl;
	std::cout << "Var2: " << structData.var2 << std::endl;
}

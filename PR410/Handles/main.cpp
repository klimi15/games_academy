#include "Pool.h"

#include <iostream>

struct Test;
using TestHandle = GamesAcademy::Handle< Test, uint32_t, 8u >;

struct Test
{
	TestHandle	handle;

	int			test1;
	int			test2;
	int			test3;
	int			test4;
};

using TestPool = GamesAcademy::Pool< Test >;

int main()
{
	TestPool pool;

	TestPool::THandle handle = pool.newObject();
	if( handle == TestPool::THandle::invalid() )
	{
		std::cout << "Failed to allocate object!\n";
		return 1;
	}

	Test* pValue = pool.getObject( handle );
	if( pValue == nullptr )
	{
		std::cout << "Failed to get object!\n";
		return 1;
	}

	pValue->test1 = 42;

	pool.deleteObject( handle );
	return 0;
}

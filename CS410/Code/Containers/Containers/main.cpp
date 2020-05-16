#include "DynamicArray.h"

#include <assert.h>

namespace GamesAcademy
{
    static int      s_result = 0;

    static void     test( bool expr );
    static int      runTest();
}

int main()
{
    return GamesAcademy::runTest();
}

static int GamesAcademy::runTest()
{
    DynamicArray< int > test1;

    test1.pushBack( 42 );
    test1.pushBack() = 1337;
    test1.pushBack( -7 );

    test( test1.getLength() == 3u );
    test( test1[ 0u ] == 42 );
	test( test1[ 1u ] == 1337 );
	test( test1[ 2u ] == -7 );

    test1.clear();

    return s_result;
}

void GamesAcademy::test( bool expr )
{
    if( expr )
    {
        return;
    }

    __debugbreak();
    s_result = 1u;
}
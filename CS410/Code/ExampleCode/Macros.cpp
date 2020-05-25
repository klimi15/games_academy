#include <iostream>

int calc( int a, int b )
{
	return a + b;
}

#define DO_STUFF( value ) ((value) + 2)
#define DO_STUFF2( funcName, ... ) ((value) + 2)

#define COMMENT( x )


#if _DEBUG
#define DEBUG( x ) std::cout << x
#else
#define DEBUG( x )
#endif

void runMacros()
{
	int result = DO_STUFF( 2 );
	// post pre processor
	int resultAfter = 2 + 2;

	COMMENT( das ist ein test )

	DEBUG( "test" );

	// result = 4;
}
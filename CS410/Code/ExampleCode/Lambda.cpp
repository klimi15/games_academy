#include <iostream>
#include <functional>

struct Vector3
{
	float x;
	float y;
	float z;
};

Vector3 FuncPtr( Vector3 value )
{
	Vector3 result = value;
	result.y += 1.0f;
	return result;
}

int callLamba( std::function< int( int ) > func, int parameter )
{
	int result = func( parameter );
	return result;
}

void runLambda()
{
	int value2 = 2;
	int value3 = 1;

	// C#
	// (int value) => { return value 1; }

	// C++03
	struct Func03
	{
		int& value2;

		Func03( int& _value2 )
			: value2( _value2 )
		{
		}

		int operator() ( int value )
		{
			return value + value2;
		}
	};

	Func03 func03 = Func03( value2 );

	// C++11
	std::function< int( int ) > func11 = [ &value2 ]( int value ) -> int
	{
		return value + value2;
	};

    value2 = 3;

    int result03 = func03( 7 );
	int result11 = func11( 7 );
	int resultCall = callLamba( func11, 7 );

    std::cout << "Result C++03: " << result03 << std::endl;
	std::cout << "Result C++11: " << result11 << std::endl;
	std::cout << "Result Call: " << resultCall << std::endl;
}

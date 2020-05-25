#include <iostream>

class NonCopyable
{
	NonCopyable(const NonCopyable&) = delete;
	NonCopyable& operator=(const NonCopyable&) = delete;
};

class PointerClass //: NonCopyable
{
public:

	PointerClass()
	{
		m_pValue = new int();
	}

	~PointerClass()
	{
		delete m_pValue;
	}

	//PointerClass( const PointerClass& other ) = delete;
	//PointerClass& operator=( const PointerClass& other ) = delete;

	int* getValue()
	{
		return m_pValue;
	}

private:

	int* m_pValue;
};

void runCopyable()
{
	PointerClass value1;
	PointerClass value2;

	value1 = value2;

	std::cout << "End" << std::endl;
}
#include <memory>

void runPointer()
{
	// C#
	//public class Stuff
	//{
	//	private int m_value;
	//
	//	public Stuff( int value )
	//	{
	//		m_value = value;
	//	}
	//
	//	public void doStuff()
	//	{
	//		m_value += 2;
	//	}
	//
	//	public int Value()
	//	{
	//		get{ return m_value; }
	//	}
	//};
	//
	//Stuff myStuff = new Stuff( 7 );
	//myStuff.doStuff();
	//int result = myStuff.Value;

	// C++
	class Stuff
	{
	public:
		int m_value;

		Stuff( int value )
		{
			m_value = value;
		}

		void doStuff()
		{
			m_value += 2;
		}

		int getValue()
		{
			return m_value;
		}
	};

	//Stuff* myStuff = new Stuff( 7 );
	//std::shared_ptr< Stuff > myStuff = std::make_shared< Stuff >( 7 );
	//myStuff->doStuff();
	//int result = myStuff->getValue();

	Stuff myStuff = Stuff( 7 );
	myStuff.doStuff();
	int result = myStuff.getValue();
}
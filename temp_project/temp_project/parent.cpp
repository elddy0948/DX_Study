#include "parent.h"

namespace temp
{
	template <class T>
	void Parent<T>::SayHello()
	{
		std::cout << "Hello from parent" << std::endl;
	}

	template <class T>
	void Parent<T>::SayBye()
	{
		std::cout << "Bye from parent" << std::endl;
	}
}
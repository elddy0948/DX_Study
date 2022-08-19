#pragma once

#include "parent.h"

namespace temp
{
	class Child : public Parent<Child>
	{
	public:
		void ChildHello();
		void ChildBye();
	};
}
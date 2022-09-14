#include "Employee.h"

#include <iostream>
#include <cstring>

Employee::Employee(char *name)
{
  std::strcpy(this->name, name);
}

void Employee::ShowYourName() const
{
  std::cout << "Name : " << name << std::endl;
}
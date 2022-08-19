#include "printer.h"

#include <iostream>

void Printer::SetString(std::string str)
{
  currentString = str;
}
void Printer::ShowString()
{
  std::cout << currentString << std::endl;
}
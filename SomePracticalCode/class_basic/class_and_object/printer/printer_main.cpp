#include "printer.h"

int main(void)
{
  Printer printer;
  printer.SetString("Hello world!");
  printer.ShowString();

  printer.SetString("I love C++");
  printer.ShowString();

  return 0;
}
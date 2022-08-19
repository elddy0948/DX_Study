#ifndef PRINTER
#define PRINTER

#include <string>

class Printer
{
private:
  std::string currentString;

public:
  void SetString(std::string str);
  void ShowString();
};

#endif
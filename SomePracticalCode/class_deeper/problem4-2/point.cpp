#include "point.h"
#include <iostream>

void Point::Init(int x, int y)
{
  xpos = x;
  ypos = y;

  return;
}

void Point::ShowPointInfo() const
{
  std::cout << '[' << xpos << ',' << ypos << ']' << std::endl;
  return;
}
#include "rectangle.h"

#include <iostream>

Rectangle::Rectangle(int width, int height)
    : width(width),
      height(height)
{
}

void Rectangle::ShowAreaInfo()
{
  std::cout << "Area : " << width * height << std::endl;
  return;
}

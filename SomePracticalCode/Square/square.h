#pragma once

#include "rectangle.h"

class Square : public Rectangle
{
public:
  Square(int width);
  ~Square() = default;
};
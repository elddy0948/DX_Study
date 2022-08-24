#include "circle.h"

#include <iostream>

bool Circle::Init(Point center, double radius)
{
  if (0 > radius)
  {
    std::cout << "radius must be higher than 0" << std::endl;
    return false;
  }
  this->radius = radius;
  this->center = center;
  return true;
}

void Circle::ShowCircleInfo() const
{
  center.ShowPointInfo();
  std::cout << "radius : " << radius << std::endl;
}

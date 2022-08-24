#include "ring.h"
#include <iostream>

void Ring::Init(const Circle &circle1, const Circle &circle2)
{
  innerCircle = circle1;
  outterCircle = circle2;
  return;
}
void Ring::ShowRingInfo() const
{
  std::cout << "Inner Circle info ..." << std::endl;
  innerCircle.ShowCircleInfo();

  std::cout << "Outter Circle info ..." << std::endl;
  outterCircle.ShowCircleInfo();
}
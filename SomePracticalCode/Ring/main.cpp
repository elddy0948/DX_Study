#include "point.h"
#include "circle.h"
#include "ring.h"

#include <iostream>

int main(void)
{
  Point pt1;
  pt1.Init(1, 1);

  Point pt2;
  pt2.Init(2, 2);

  Circle circle1;
  circle1.Init(pt1, 4);

  Circle circle2;
  circle2.Init(pt2, 9);

  Ring ring;
  ring.Init(circle1, circle2);
  ring.ShowRingInfo();
}

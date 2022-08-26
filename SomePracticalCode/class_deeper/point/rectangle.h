#ifndef __RECTANGLE_H_
#define __RECTANGLE_H_

#include "point.h"

class Rectangle
{
private:
  Point upLeft;
  Point lowRight;

public:
  Rectangle(const int &x1, const int &y1, const int &x2, const int &y2);
  void ShowRectInfo() const;
};

#endif
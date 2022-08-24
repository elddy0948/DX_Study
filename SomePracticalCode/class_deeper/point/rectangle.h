#ifndef __RECTANGLE_H_
#define __RECTANGLE_H_

#include "point.h"

class Rectangle
{
private:
  Point upLeft;
  Point lowRight;

public:
  bool InitMembers(const Point &upLeft, const Point &lowRight);
  void ShowRectInfo() const;
};

#endif
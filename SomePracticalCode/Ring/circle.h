#ifndef __CIRCLE_H_
#define __CIRCLE_H_

#include "point.h"

class Circle
{
private:
  Point center;
  double radius;

public:
  bool Init(Point center, double radius);
  void ShowCircleInfo() const;
};

#endif
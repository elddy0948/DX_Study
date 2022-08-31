#ifndef __RING_H_
#define __RING_H_

#include "circle.h"

class Ring
{
private:
  Circle innerCircle;
  Circle outterCircle;

public:
  void Init(const Circle &circle1, const Circle &circle2);
  void ShowRingInfo() const;
};

#endif
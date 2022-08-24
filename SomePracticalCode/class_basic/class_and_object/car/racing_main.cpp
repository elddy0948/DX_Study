#include "car.h"

int main(void)
{
  Car run99; // Object

  run99.InitMembers("run99", 100);
  run99.Accel();
  run99.Accel();
  run99.Accel();

  run99.ShowCarState();
  run99.Break();

  run99.ShowCarState();

  return 0;
}
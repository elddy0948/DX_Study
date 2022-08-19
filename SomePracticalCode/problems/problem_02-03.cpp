#include <iostream>

using namespace std;

typedef struct __Point
{
  int xpos;
  int ypos;
} Point;

Point &PntAddr(const Point &p1, const Point &p2)
{
  int newXPos = p1.xpos + p2.xpos;
  int newYPos = p1.ypos + p2.ypos;

  Point *newPoint = new Point;
  newPoint->xpos = newXPos;
  newPoint->ypos = newYPos;
  return *newPoint;
}

int main(void)
{
  Point *p1 = new Point;
  p1->xpos = 3;
  p1->ypos = 5;

  Point *p2 = new Point;
  p2->xpos = 4;
  p2->ypos = 10;

  Point newPointer = PntAddr(*p1, *p2);

  cout << newPointer.xpos << endl;
  cout << newPointer.ypos << endl;
  return 0;
}
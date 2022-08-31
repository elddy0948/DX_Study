#include <iostream>
#include "rectangle.h"
using namespace std;

Rectangle::Rectangle(const int &x1, const int &y1, const int &x2, const int &y2)
    : upLeft(x1, y1), lowRight(x2, y2)
{
  // Empty
}

void Rectangle::ShowRectInfo() const
{
  cout << "Up Left : " << '[' << upLeft.GetX() << ',';
  cout << upLeft.GetY() << ']' << endl;
  cout << "Low Right : " << '[' << lowRight.GetX() << ',';
  cout << lowRight.GetY() << ']' << endl;
}
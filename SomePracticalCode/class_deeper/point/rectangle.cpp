#include <iostream>
#include "rectangle.h"
using namespace std;

bool Rectangle::InitMembers(const Point &upLeft, const Point &lowRight)
{
  if ((upLeft.GetX() > lowRight.GetX()) || (upLeft.GetY() > lowRight.GetY()))
  {
    cout << "잘못된 위치정보 전달" << endl;
    return false;
  }
  this->upLeft = upLeft;
  this->lowRight = lowRight;
  return true;
}

void Rectangle::ShowRectInfo() const
{
  cout << "좌 상단 : " << '[' << upLeft.GetX() << ',';
  cout << upLeft.GetY() << ']' << endl;
  cout << "우 하단 : " << '[' << lowRight.GetX() << ',';
  cout << lowRight.GetY() << ']' << endl;
}
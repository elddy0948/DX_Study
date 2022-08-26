#include <iostream>
#include "point.h"
#include "rectangle.h"

using namespace std;

int main(void)
{
  Rectangle rect(1, 1, 5, 5);
  rect.ShowRectInfo();
  return 0;
}
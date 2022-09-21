#include <iostream>

#include "ArrayTemplate.h"
#include "Point.h"

using namespace std;

int main(void)
{
  BoundCheckArray<int> int_arr(5);

  for (int i = 0; i < 5; i++)
    int_arr[i] = (i + 1) * 11;
  for (int i = 0; i < 5; i++)
    cout << int_arr[i] << endl;

  BoundCheckArray<Point> point_arr(3);
  point_arr[0] = Point(3, 4);
  point_arr[1] = Point(5, 6);
  point_arr[2] = Point(7, 8);

  for (int i = 0; i < point_arr.GetArrLength(); i++)
  {
    cout << point_arr[i];
  }

  typedef Point *POINT_PTR;

  BoundCheckArray<POINT_PTR> pointer_arr(3);

  pointer_arr[0] = new Point(3, 4);
  pointer_arr[1] = new Point(5, 6);
  pointer_arr[2] = new Point(7, 8);

  for (int i = 0; i < pointer_arr.GetArrLength(); i++)
  {
    cout << *(pointer_arr[i]);
  }

  delete pointer_arr[0];
  delete pointer_arr[1];
  delete pointer_arr[2];

  return 0;
}
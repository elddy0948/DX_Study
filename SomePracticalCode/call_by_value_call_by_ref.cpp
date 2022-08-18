#include <iostream>

using namespace std;

int Adder(int lhs, int rhs)
{
  return lhs + rhs;
}

void SwapByValue(int lhs, int rhs)
{
  int temp = lhs;
  lhs = rhs;
  rhs = temp;
  // Call by value
}

void SwapByRef(int *lhs, int *rhs)
{
  int temp = *lhs;
  *lhs = *rhs;
  *rhs = temp;
  // Call by reference
}

void SwapByRef2(int &lhs, int &rhs)
{
  int temp = lhs;
  lhs = rhs;
  rhs = temp;
  // Call by reference
}

int main(void)
{
  int val1 = 10;
  int val2 = 20;

  // SwapByValue(10, 20);
  // SwapByRef(&val1, &val2);
  SwapByRef2(val1, val2);

  cout << "val 1 : " << val1 << endl;
  cout << "val 2 : " << val2 << endl;
}
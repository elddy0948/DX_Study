#include <iostream>

using namespace std;

void MakeNegative(int &num)
{
  num *= -1;
}

void IncreaseOne(int &num)
{
  num++;
}

void SomeFunc(int *lhs, int *rhs)
{
  int temp = *lhs;
  *lhs = *rhs;
  *rhs = temp;
}

int main(void)
{

  int val = 10;

  IncreaseOne(val);
  cout << val << endl;

  MakeNegative(val);
  cout << val << endl;

  int num1 = 5;
  int *ptr1 = &num1;
  int num2 = 10;
  int *ptr2 = &num2;

  cout << *ptr1 << endl;
  cout << *ptr2 << endl;

  SomeFunc(ptr1, ptr2);

  cout << *ptr1 << endl;
  cout << *ptr2 << endl;

  return 0;
}
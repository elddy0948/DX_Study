#include <iostream>

using namespace std;

int main(void)
{
  int num = 12;
  int *ptr = &num;
  int **dptr = &ptr;

  cout << "ptr is pointing : " << ptr << endl;
  cout << "where num is living : " << &num << endl;
  cout << "where ptr living : " << &ptr << endl;
  cout << "what's in ptr : " << *ptr << endl;

  cout << "------------ dptr -------------" << endl;

  cout << dptr << endl;
  cout << &dptr << endl;
}
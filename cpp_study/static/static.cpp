#include <iostream>

using namespace std;

static int globalVar = 0;

class MyClass
{
private:
  static int classCounter;

public:
  MyClass()
  {
    classCounter++;
    cout << classCounter << " MyClass ! " << endl;
  }
};

int MyClass::classCounter = 0;

int main(void)
{
  MyClass m1;
  MyClass m2;

  MyClass();

  return 0;
}
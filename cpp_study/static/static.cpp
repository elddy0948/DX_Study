#include <iostream>

using namespace std;

static int globalVar = 0;

class MyClass
{
private:
  static int classCounter;
  int someVar = 0;
  const static int initializeInteger = 0;

public:
  MyClass()
  {
    classCounter++;
    cout << classCounter << " MyClass ! " << endl;
  }

  static void MyFunc()
  {
    // someVar++; // error!
    classCounter++;
    return;
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
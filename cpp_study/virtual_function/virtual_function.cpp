#include <iostream>

class B
{
public:
  void BFunc()
  {
    std::cout << "B Func()!" << std::endl;
  }
};

class BB : public B
{
public:
  void BFunc()
  {
    std::cout << "BB Func()!" << std::endl;
  }
};

class A
{
public:
  virtual void MyFunc()
  {
    std::cout << "A MyFunc()" << std::endl;
  }
};

class AA : public A
{
public:
  virtual void MyFunc()
  {
    std::cout << "AA MyFunc()" << std::endl;
  }
};

class AAA : public AA
{
public:
  virtual void MyFunc()
  {
    std::cout << "AAA MyFunc()" << std::endl;
  }
};

int main(void)
{
  B *bClass = new BB();
  bClass->BFunc(); // B Func()!

  bClass = new B();
  bClass->BFunc(); // B Func()!

  BB *bbClass = new BB();
  bbClass->BFunc(); // BB Func()!

  A *aClass = new AA();
  aClass->MyFunc(); // AA MyFunc()!

  AA *aaClass = new AAA();
  aaClass->MyFunc(); // AAA MyFunc()!

  AAA *aaaClass = new AAA();
  aaaClass->MyFunc(); // AAA MyFunc()!

  A *someA = new A();
  someA->MyFunc();
  delete someA;

  someA = new AA();
  someA->MyFunc();
  delete someA;

  return 0;
}
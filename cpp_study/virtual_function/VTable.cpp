#include <iostream>

class AA
{
public:
  void Func()
  {
    std::cout << "AA::Func() " << std::endl;
  }

  virtual void VFunc()
  {
    std::cout << "AA::VFunc() " << std::endl;
  }
};

class B : public AA
{
public:
  virtual void VFunc()
  {
    std::cout << "BB::VFunc() " << std::endl;
  }

  void BFunc()
  {
    std::cout << "BB::BFunc() " << std::endl;
  }
};

int main()
{
  AA *aaClass = new AA();

  aaClass->Func();
  aaClass->VFunc();

  aaClass = new B();

  aaClass->Func();
  aaClass->VFunc();

  B *bClass = new B();

  bClass->Func();
  bClass->VFunc();
  bClass->BFunc();

  return 0;
}
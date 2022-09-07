
class MyClass
{
private:
  int myVar;

public:
  explicit MyClass(int number) : myVar(number) {}

  int GetFoo() { return myVar; }
};

void SomeFunction(MyClass myClass)
{
  int number = myClass.GetFoo();
}

int main()
{
  // SomeFunction(10);

  // MyClass m1 = 1;
}
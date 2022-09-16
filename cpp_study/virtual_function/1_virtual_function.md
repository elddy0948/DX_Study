# Virtual Function

기본적으로 C++ 컴파일러는 포인터 연산의 가능성을 판단할 때, **포인터의 자료형**을 기준으로 판단하지, 실제 가리키는 객체의 자료형을 기준으로 판단하지 않습니다.
예를들어

```cpp
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
```

클래스 B, 클래스 BB가 있다고 하면,

```cpp
int main(void)
{
  B *bClass = new BB();
  bClass->BFunc(); // B Func()!

  bClass = new B();
  bClass->BFunc(); // B Func()!

  BB *bbClass = new BB();
  bbClass->BFunc(); // BB Func()!

  return 0;
}
```

위에서 보이는것과 같이, bClass는 `new BB();`로 초기화했음에도 불구하고, B클래스의 함수인 B Func()! 이 출력되는 것을 확인할 수 있습니다. 바로 이 예시가 C++ 컴파일러가 포인터의 자료형을 기준으로 판단한다는 것을 보여주기 위한 예시입니다.

하지만 반대로 실제 가리키는 객체의 자료형을 기준으로 판단하게 만들 수 있는데 이때 사용되는 것이 **virtual** 입니다.

## What is 'Virtual Function'?

'가상함수'는 C++의 개념이 아닌 객체지향의 개념입니다. C++ 에서는 **virtual** 키워드를 함수 앞에 붙여서 가상함수를 선언하고, 선언하고나면 이 함수를 오버라이딩 하는 함수도 가상함수가 됩니다.

예를들어,

```cpp
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
```

위의 B, BB클래스와 동일하지만, 이번에는 `virtual`을 사용하였습니다.

```cpp
  A *aClass = new AA();
  aClass->MyFunc(); // AA MyFunc()!

  AA *aaClass = new AAA();
  aaClass->MyFunc(); // AAA MyFunc()!

  AAA *aaaClass = new AAA();
  aaaClass->MyFunc(); // AAA MyFunc()!
```

이번에는 A 자료형으로 선언한 aClass가 A클래스의 MyFunc()을 호출하지 않고, 가리키고 있는 객체인 AA 클래스의 MyFunc()에 접근하는 것을 확인할 수 있습니다.

정리해보면 non-virtual function은 **포인터 변수의 자료형** 을 기준으로, virtual function은 가리키고 있는 객체의 자료형을 기준으로 판단한다는 것을 알 수 있습니다.

## Pure Virtual Function?

순수가상함수(Pure virtual function)은 함수의 몸체가 정의되지 않은 함수를 의미합니다. 이는 가상함수에 0을 대입해서 컴파일러에게 알려줄 수 있습니다.

```cpp
virtual void ShowSomething() const = 0;
```

이런식으로 나타내어 컴파일러에게 "Body를 정의하지 않았다!"를 알려줄 수 있습니다.

### Why use it?

이 virtual function을 잘 보고 있으면, 바로 객체지향의 중요한 요소인 **Polymorphism**이 떠오릅니다. 이 Polymorphism 즉, 다형성은 똑같은 문장인데 결과가 다르게 나오는,

```cpp
  A *someA = new A();
  someA->MyFunc();
  delete someA;

  someA = new AA();
  someA->MyFunc();
  delete someA;
```

똑같은 MyFunc()을 호출하는데 위의 MyFunc()은 "A MyFunc()", 아래의 MyFunc()은 "AA MyFunc()"이 나올 것입니다. 그렇기 때문에 virtual function을 활용하여 C++에서의 '다형성'을 효율적으로 활용할 수 있습니다.

## Virtual Destructure

소멸자에도 virtual이 아주 중요하게 작용합니다.

```cpp
class A
{
public:
  ~A() { ... }
};

class AA : public A
{
public:
  ~AA() { ... }
};

int main()
{
  A* aClass = new AA();
  delete aClass;
}
```

만약 virtual이 없는 소멸자를 만들어 main에서와 같이 호출한다면, 위에서 살펴봤듯, 컴파일러는 포인터의 자료형에 관심이 있습니다. 그렇기 때문에 aClass가 소멸되면, A 클래스의 소멸자만 실행됩니다.

이를 해결하기 위해 virtual 키워드를 사용해줍니다.

```cpp
class A
{
public:
  virtual ~A() { ... }
};

class AA : public A
{
public:
  virtual ~AA() { ... }
};

int main()
{
  A* aClass = new AA();
  delete aClass;
}
```

virtual 키워드를 통해 객체 소멸 과정에서 A 소멸자가 호출되는데, virtual이므로 AA 소멸자로 넘어갑니다.
AA 소멸자를 실행한 후 이어서 A 소멸자를 실행합니다.
즉, 모든 소멸자를 호출하게 됩니다.

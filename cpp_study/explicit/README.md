# Explicit

## What is explicit?

explicit은 한국말로 '명시적' 이라는 뜻을 가지고 있습니다.
그래서 constructor, conversion function, deduction guide에 `explicit` 지정자를 붙인다는 의미는, 그것에 대한 암묵적 형변환(implicit-conversions)이나 복사 초기화(copy-initialization)를 제한하겠다는 의미입니다.

### Implicit Conversions?

C++ 컴파일러는 함수의 인자로 들어오는 값을 알아서 타입에 맞게 형변환 해줄 수 있습니다.
예를들어

```cpp
class MyClass
{
private:
  int myVar;

public:
  MyClass(int number) : myVar(number) {}

  int GetFoo() { return myVar; }
};

void SomeFunction(MyClass myClass)
{
  int number = myClass.GetFoo();
}

int main()
{
  SomeFunction(10);
}
```

위의 예시에서 SomeFunction을 호출하는데 SomeFunction 함수의 인자의 타입은 MyClass인데, 이미 MyClass의 생성자가 int 타입을 받아 자동적으로 초기화 해주는것을 알고있기때문에, 컴파일러는 `main` 에서 SomeFunction 함수를 호출할 때, int 타입인 10을 넘겨줘도 컴파일러가 암묵적으로 형변환을 하여 생성해줍니다.

이때 만약에 `constructor`에 `explicit` 키워드를 붙여준다면, SomeFunction의 호출 방법은 `SomeFunction(MyClass(10));` 이런 방식으로 호출될 수 있을것입니다.

### Copy Initialization?

copy initialization도 암묵적 형변환과 똑같습니다. 코드를 간단하게 살펴보면,

위에서 살펴보았던 `MyClass` 클래스는 다음과 같이 초기화가 가능합니다.

```cpp
MyClass m1 = 1;
```

그럼 컴파일러는 저 `= 1` 이 의미하는 것이 암묵적으로 `MyClass(int number)` 라는 생성자의 인자에 저 int형 1이라는 숫자를 넣으라는 소리구나. 하고 형변환을하여 초기화 합니다. 역시 `constructor`에 `explicit` 키워드를 붙이면 해당 줄은 error가 발생하게 됩니다.  

# Exception Handling

## C++ 의 예외처리 메커니즘

한줄로 정리해보면, `throw`가 예외를 발생시키고, `try`가 이를 감지해서, `catch`에서 처리한다. 라고 할 수 있습니다.
즉, `throw`는 예외가 발생하는 것을 알리는 문장에 사용됩니다.
`try`는 예외가 발생할 수 있는 일련의 코드들을 묶어두고, 예외가 발생했을 때, 이를 감지하는 역할을 합니다.
`catch`는 `try`에서 발생된 예외를 받아와 이를 처리하는 코드를 작성할 때 사용됩니다.

```cpp
try
{
  if ( 예외 발생 ! )
  {
    throw expn;
  }
}
catch (type exn)
{
  // Handling exceptions
}
```

이런 형식의 코드가 작성됩니다.
'try 와 catch는 항상 **이어서** 등장해야 합니다!'

그리고 `throw`해준 자료형과, `catch`로 받는 자료형이 동일해야 합니다.

### 예외를 던져주는 곳과 처리하는 곳을 다르게 하고싶다?

예를들어 이런 상황을 보겠습니다.

```cpp
void CallFunc()
{
  // ...
  throw exception;
}

try
{
  CallFunc();
}
catch (type err)
{
  // do something
}
```

위의 코드 같은 경우에는 `CallFunc()`에는 try-catch문이 존재하지 않습니다. 하지만 예외를 throw해주고 있는데, 이 throw한 예외 정보는 바로 `CallFunc()`을 호출한 부분에 있는 try-catch문에서 처리가 됩니다.

이를 **Stack Unwinding**이라고 합니다.
이런 이름이 붙은 이유는 바로 함수가 호출되고, 종료되는 과정을 보면 이해가 될 수 있습니다. 함수는 호출되고 종료될 때, Call Stack에서 스택에 쌓였다가, 종료되면 스택에서 사라지고 그 아래에 쌓여 있던 함수로 돌아가게 됩니다.
이 과정을 생각해보면, call stack에서 제일 위에 함수에서, 예외가 발생하여 함수가 종료되면, 그 아래에 있던 함수로 예외에 대한 정보가 전달되게 됩니다.
그래서 이 과정을 'Stack Unwinding'이라고 합니다.

보통 제일 아래에는 `main`함수가 있는데, 만약 이 main함수 까지 와도 예외처리가 되지 않으면, 프로그램을 종료해버립니다.

### 자료형이 일치하지 않으면?

`throw` 해서 넘겨주는 자료형과 `catch`로 받는 자료형이 일치하지 않는다면, 해당 `catch`문은 건너뛰게 됩니다. 그러면 함수를 호출 한 부분으로 예외를 넘겨주거나, 또는  `catch`문을 여러번 선언하여 사용해서 처리하는 방법도 있습니다.

```cpp
try
{
  // ...
  throw -1;
}
catch (char ch)
{
  // ...
}
catch (int num)
{
  // ...
}
```

만약 위의 코드 같은 경우에는 -1 이라는 예외가 던져졌을때, `catch(int num)` 부분에서 예외가 처리됩니다.

### 전달되는 예외 타입 명시

```cpp
int ThrowExceptionFunc() throw (int, char)
{
  // ...
}
```

이런식으로 전달될 수 있는 예외 타입들에 대한 명시를 해줄 수 있습니다. `throw (int, char)`의 의미는 int 자료형과 char 자료형의 예외가 전달될 수 있다는 의미이며, 만약 그 외의 자료형으로 예외가 전달되면, 프로그램이 종료되게 됩니다.

`throw ()`처럼 비워두면, 어떠한 예외도 전달되지 않을 것이라는 의미입니다. 만약 예외가 전달된다면, 프로그램은 종료되게 됩니다.

## 예외 클래스 정의

예외 클래스를 만들어, 예외 객체를 생성하여 예외를 표현하는 방법도 있습니다.

```cpp
class SomeException
{
private:
  int num;

public:
  SomeException(int num) : num(num) {}
  void ShowMessage()
  {
    cout << "Exception with " << num << endl;
  }
};

// ... 

try
{
  SomeException expn(10);
  throw expn;
}
catch (SomeException& expn)
{
  expn.ShowMessage();
}
```

이런식으로 예외 상황을 잘 표현할 수 있게 클래스를 만들어서 예외를 처리합니다.

### 예외 클래스의 상속

예외 클래스는 상속이 가능합니다.

```cpp
class A
{
public:
  virtual void SomeMessage() = 0;
};

class B : public A
{
private: 
  int num;

public:
  B(int num) : num (num) {}
  void SomeMessage()
  {
    cout << "B : " << num;
  }
};

class C : public A
{
private:
  int num;

public:
  C(int num) : num(num) {}
  void SomeMessage()
  {
    cout << "C : " << num;
  }
};
```

이런식으로 예외 클래스들을 하나의 클래스인 'A' 로 묶는 것이 가능해집니다.

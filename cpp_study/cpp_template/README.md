# Template

## 함수 템플릿

함수의 **기능**은 결정되어 있지만, 자료형은 결정되지 않은 함수를 말합니다.

```cpp
template <typename T>
T Sum(T lhs, T rhs)
{
  return lhs + rhs;
}
```

### 템플릿 함수

이 **템플릿 함수**는 함수 템플릿에 의해 만들어지는 함수입니다. 즉, 함수 템플릿에서 자료형이 정해진 함수라고 할 수 있습니다.
만약 한번 만들어졌다면, 다음에 똑같은 자료형으로 함수를 호출할때에는 다시 만들지 않고, 만들어 둔 함수를 호출하게 됩니다.

**함수 템플릿** -> **템플릿 함수** 과정은 컴파일 시간에 진행이되는데, 이는 '컴파일 시간'을 느리게 하는 것이지, '실행 시간'을 느리게 하는 것은 아닙니다.

### 함수 템플릿의 특수화 (Specialization)

상황에 따라서 템플릿 함수의 '구현'이 달라져야할 때가 있습니다.
예를들어

```cpp
template <typename T>
T Max(T lhs, T rhs)
{
  return lhs > rhs ? lhs : rhs;
}
```

이런 템플릿 함수가 있다고 가정하겠습니다. 이는 정수나 실수를 비교하여 큰 값을 가져오는 방식에는 잘 동작하지만, **문자열과 같은 자료형에게는 잘 맞지 않는 구조를 가지고 있습니다.** 즉, 문자열의 크기를 비교하고싶을때는 의미가 없어지게 되는것입니다.

이때 사용하는것이 Specialization 입니다.

```cpp
template<>
const char* (const char* lhs, const char* rhs)
{
  return strlen(lhs) > strlen(rhs) ? lhs : rhs;
}
```

이런식으로 사용하게 된다면, 컴파일러는 `const char*` 타입에 대해서는 함수 템플릿을 보고 새로 생성하지 않고, 이미 정의된 위의 코드로 사용하게 되는것입니다.
즉, 개발자가 "const char* 타입은 새로 만들지 말고, 이거 써라!" 라고 컴파일러에게 알려주는것입니다.

## 클래스 템플릿

함수 템플릿과 전체적인 개념은 유사합니다. 클래스 템플릿을 통해 템플릿 클래스를 생성합니다.

주의해야할 점은

```cpp
SomeClass<int> sc;
```

여기서 자료형을 명시해주는 `<int>`는 생략할 수 없습니다. 반드시 자료형을 명시해주어야 합니다.

### 선언과 정의 분리

```cpp
template <typename T>
class A
{
public:
  T A(const T& ref);
};
```

```cpp
template<typename T>
T A<T>::A(const T& ref)
{
  // ...
}
```

이렇게 클래스 템플릿의 멤버함수를 클래스 외부에 정의할 때에는 위의 예시와 같이 `T`에 대해 템플릿화 된 `A`클래스 템플릿 이라는 명시를 해주어야 합니다.

그리고 컴파일에 대한 이해도 중요합니다. 컴파일은 파일 단위로 이루어지고, 이에 따라 가끔 템플릿 클래스를 사용할 때, 클래스 템플릿에 대한 모든 정보(클래스 템플릿의 선언과 정의)를 주지 않아서 컴파일 에러를 마주할 때가 있습니다.
꼭 파일을 분리해서 사용할때에는 클래스 템플릿의 정보를 완전히 준 상태에서 템플릿 클래스를 사용해야합니다.

## 클래스 템플릿 자료형과 friend

템플릿 클래스 객체를 선언할 때, 이때까지 봤었던 것들은 대부분이 `SomeClass<int> sc;` 의 형태였습니다. 하지만 다음과 같은 형태도 가능합니다.

```cpp
template <typename T>
Class A { ... };

template <typename T>
Class B { ... };

A<B<int>> ab; // OK

typedef B<int>* B_PTR;
A<B_PTR> bpA; // OK
```

또한 자료형이 이미 정해진 클래스 템플릿에 대해서 `friend` 함수 선언도 가능합니다.

```cpp
template<typename T>
class Point
{
  // ...
  friend Point<int> operator+(const Point<int>&, const Point<int>&);
};
```

## 클래스 템플릿의 특수화

함수 템플릿과 마찬가지로 클래스 템플릿 또한 특수화가 가능합니다.
**함수 템플릿과 똑같이, 어떠한 자료형을 기반으로 생성된 객체에 대해 구분이 되는 다른 행동을 따로 구현해주고 싶을때 사용**합니다.

```cpp
template <typename T>
class A
{
public:
  T A(T num) { ... }
};

template<>
class A<int>
{
public:
  int A(int num) { ... }
};
```

### 클래스 템플릿의 부분 특수화

다음과 같은 클래스 템플릿이 있을때

```cpp
template <typename T1, typename T2>
class A{ ... }

template <>
class A<int, char> { ... } // 전체 특수화

template <typename T1>
class A<T1, int> { ... } // 부분 특수화
```

제일 아래와 같이 둘중 하나만 자료형을 명시한 것을 부분 특수화라고 합니다.

**호출이될때에는 전체 특수화가 부분 특수화보다 우선시됩니다.**

## 템플릿 인자

```cpp
template <typename T>
```

에서 `T`는 '템플릿 매개변수'를 뜻하고, 여기에 들어오는 자료형을 '템플릿 인자' 라고 합니다.

템플릿 매개변수에는 변수의 선언이 올 수 있습니다.

```cpp
template <typename T, int len>
class A { ... };

int main()
{
  A<int, 5> a;
}
```

위와 같은 형식으로 사용될 수 있고, 여기서 상수가 달라지면 서로 다른 자료형을 가진것이 됩니다.
즉, `A<int, 5> != A<int, 7>` 이라는 의미입니다.

### 디폴트 값 지정

디폴트 값도 지정 가능합니다.

```cpp
template <typename T = int, int len = 5>
class A { ... };

int main()
{
  A<> a;
}
```

디폴트 값을 사용하더라도 `<>`는 꼭 붙여주어야 합니다.

## 템플릿과 static

### 함수 템플릿 static

결론부터 말하면, 함수 템플릿의 `static` 지역 변수는 **템플릿 함수별로 각각 존재**하게 됩니다.

```cpp
template <typename T>
void Show()
{
  static T num = 0;
}

void Show<int>()
{
  static int num = 0;
}

void Show<double>()
{
  static double num = 0;
}
```

### 클래스 템플릿 static

함수 템플릿과 마찬가지로, 각 클래스 별로 `static` 멤버 변수를 유지하게 됩니다.

## typename T vs ' '

템플릿을 쓰다보면

```cpp
template <typename T>

template <>
```

이 두가지를 볼 수 있습니다. 각각은 과연 언제 사용되어야 할까요?

우선 `typename T`같은 경우에는 템플릿 정의이긴 한데, T라는 타입을 사용해야할 때 사용되어야 합니다.

```cpp
template <typename T>
T A<T>::num = 0;
```

다음 빈칸으로 두는 경우에는 템플릿 정의이긴 한데, T라는 타입은 필요없고, 이미 자료형이 정의되어있는 것을 나타낼 때 사용합니다.

```cpp
long A<long>::num = 0;
```

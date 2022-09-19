# Assignment operator overloading

## Assignment operator overloading and copy constructor

대입연산자 오버로딩은 복사생성자와 성격이 유사합니다.

### 대입연산자 오버로딩 특성

- 정의하지 않으면 디폴드 대입연산자가 삽입됩니다.
- 디폴트 대입 연산자는 멤버 대 멤버의 복사(얕은 복사)를 진행합니다.
- 연산자 내에서 동적 할당을 한다면, 그리고 깊은 복사가 필요하다면 직접 정의해야 합니다.

여기서 '대입연산자', '연산자' 키워드를 '복사생성자', '생성자'로 바꾸기만 하면 '복사생성자'의 특성이 됩니다.

### 복사 생성자와의 차이점

복사 생성자와 대입 연산자의 차이점으로는 **호출되는 시점**에 있습니다.

- 복사 생성자가 호출되는 대표적인 예시

```cpp
MyClass mc();
MyClass mc2 = mc;
```

- 대입 연산자가 호출되는 대표적이 예시

```cpp
MyClass mc1();
MyClass mc2();

mc2 = mc1;
```

위의 두가지 예시에서 눈여겨 봐야할 점은, 대입 연산자가 호출될 때는, **이미 생성 및 초기화가 진행된 객체**에 대해서 대입연산 시에 대입 연산자가 호출됩니다.
위의 `mc1`, `mc2`는 이미 생성된 객체이고, 이 둘 사이에서 대입 연산이 일어났으니, 대입 연산자가 호출됩니다.

## 디폴트 대입 연산자의 문제점

### Shallow copy (얕은 복사)로 인해 일어나는 문제들

디폴트 대입 연산자의 문제점은 보통 얕은 복사로 인해 발생하는 문제들입니다.

```cpp
class Person
{
public:
  Person(char *name)
  {
    this->name = new char[strlen(name) + 1];
    strcpy(this->name, name);
  }

  void ShowName()
  {
    std::cout << name << std::endl;
  }

  ~Person()
  {
    delete[] name;
    std::cout << "~Person()" << std::endl;
  }

private:
  char *name;
};

int main(void)
{

  Person personOne("Joons");
  Person personTwo("Holuck");

  personTwo = personOne;

  personOne.ShowName();
  personTwo.ShowName();

  return 0;
}
```

이 코드의 출력은

```cpp
Joons
Joons
~Person()
```

입니다. 우선 문제점을 하나하나 살펴보면 `personTwo = personOne` 부분에서 personTwo가 가리키고 있던 "Holuck" 이라는 문자열은 메모리 공간에 남아있는데, 이를 아무도 가리키지 않는 상태가 됩니다. 즉, 메모리 누수가 발생하게 됩니다.
또한 출력에서 '~Person()' 이 한번만 호출되었습니다. `personTwo`가 소멸되면서 `delete[] name` 문장이 실행되고, personOne과 personTwo는 얕은 복사로 인해 서로 같은 주소의 "Joons"문자열을 가리키고 있기에, 이 "Joons" 문자열은 `personTwo`가 소멸되면서 지워버립니다.
그러면 다음에 `personOne`이 소멸할 때, 이미 소멸된 것을 또 소멸시키려고하는 문제가 발생하게 됩니다.

즉, **메모리 누수**와 **중복 소멸**의 문제가 있습니다.

살펴본 두가지 문제점을 바탕으로 코드를 개선해보겠습니다.

```cpp
  Person &operator=(const Person &ref)
  {
    // 이미 가리키고 있던 name을 해제시켜줌
    delete[] name;

    // Deep copy
    int len = strlen(ref.name) + 1;
    name = new char[len];
    strcpy(name, ref.name);
    return *this;
  }
```

이렇게 대입 연산자 오버로딩을 사용하면 코드가 정상적으로 동작하는것을 확인할 수 있습니다.

## 이니셜라이저를 통한 성능 향상

이니셜라이저를 사용 하고 안하고의 차이를 확인해보겠습니다.

```cpp
class BBB
{
private:
  AAA mem;
public:
  BBB(const AAA& ref) : mem(ref) {}
};

class CCC
{
private:
  AAA mem;
public:
  CCC(const AAA& ref) { mem = ref; }
};
```

위의 `BBB`, `CCC` 클래스는 각각 이니셜라이저를 사용한 클래스, 하지 않은 클래스입니다.

```cpp
AAA objA();
BBB objB(objA);
CCC objC(objA);
```

이런식으로 생성하면, BBB는, AAA의 복사생성자 `AAA(const AAA& ref)`만 호출되는 것을 확인할 수 있고,
CCC의 경우는 `AAA(int n=0)` 그리고 `operator=(const AAA& ref)` 이렇게 생성자 하나와 대입 연산자까지 호출되는 것을 확인할 수 있습니다.

이니셜라이저를 사용하면 선언과 동시에 초기화가 이루어지는 형태로 바이너리 코드가 생성되기 때문에, 이니셜라이저를 사용한다면, 좀 더 성능에서 이점을 가져갈 수 있습니다.

# Friend

## Class의 Friend 선언

Friend! 그대로 해석하면 친구라는 의미입니다. 사람들 사이에서도 진정한 친구끼리면 비밀도 나눌 수 있는 사이가 될 수 있습니다. 이런 의미처럼 클래스에서의 `Friend` 키워드는 어떤 클래스의 `private` 멤버에게도 접근할 수 있는 권한을 줄 수 있습니다.

```cpp
class HoLuck;

class Joons
{
private:
  int favNumber;
  friend class HoLuck;
  // ...
};

class HoLuck
{
  // ...
public:
  // ...
  void ShowFriendFavNumber(Joons &frd);
};

void HoLuck::ShowFriendFavNumber(Joons &frd)
{
  std::cout << "HoLuck's Friend Joons fav number : " << frd.favNumber;
}
```

위의 코드에서 보면 `Joons` 클래스는 `HoLuck` 클래스를 friend로 설정해주었고, 덕분에 ShowFriendFavNumber 메서드의 구현부를 보면 `frd.favNumber` 즉 `Joons` 클래스의 private 멤버 변수인 `favNumber` 에 접근할 수 있게 됩니다.

## Function의 Friend 선언

Function의 `friend` 키워드도 클래스의 friend와 같은 역할을 합니다. friend가 붙여진 함수가 속해있는 클래스의 private 멤버에 접근할 수 있게 됩니다.

```cpp
class Joons
{
private:
  int favNumber;
  // ...
  
public:
  // ...
  void ShowFriendFavNumber(HoLuck &);
};

class HoLuck
{
private:
  int favNumber;

public:
  // ...
  friend void Joons::ShowFriendFavNumber(HoLuck &);
};

void Joons::ShowFriendFavNumber(HoLuck &frd)
{
  std::cout << "Joon's friend HoLuck's fav number is " << frd.favNumber << std::endl;
}

```

위의 코드에서 확인할 수 있듯 `Joons::ShowFriendFavNumber` 를 `HoLuck` 클래스에 friend 로 등록해주면서 HoLuck의 `favNumber` 에 접근할 수 있는 권한이 생깁니다.

## 기타

friend 선언은 OOP의 대명사 중 하나인 '정보은닉'을 무너뜨리는 기법입니다. (private은 접근을 막으려고 붙여주는건데 거기에 접근을 할 수 있게 해주기 때문)
그렇기 때문에 friend 의 사용은 소극적으로 사용해주어야 하며, 꼭 필요한 상황이 아니라면 가급적이면 사용하지 않는 것이 좋습니다.

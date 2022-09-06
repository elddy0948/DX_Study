#include <iostream>

using namespace std;

class HoLuck;

class Joons
{
private:
  int favNumber;
  friend class HoLuck;

public:
  Joons(int num) : favNumber(num)
  {
  }
  void ShowFriendFavNumber(HoLuck &);
};

class HoLuck
{
private:
  int favNumber;

public:
  HoLuck(int num) : favNumber(num)
  {
  }
  void ShowFriendFavNumber(Joons &frd);
  friend void Joons::ShowFriendFavNumber(HoLuck &);
};

void HoLuck::ShowFriendFavNumber(Joons &frd)
{
  std::cout << "HoLuck's Friend Joons fav number : " << frd.favNumber << std::endl;
}

void Joons::ShowFriendFavNumber(HoLuck &frd)
{
  std::cout << "Joon's friend HoLuck's fav number is " << frd.favNumber << std::endl;
}

int main(void)
{
  Joons joons(10);
  HoLuck holuck(8);

  holuck.ShowFriendFavNumber(joons);
  joons.ShowFriendFavNumber(holuck);

  return 0;
}
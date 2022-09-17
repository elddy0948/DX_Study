#include <iostream>

using namespace std;

class Point
{
public:
  Point(int x = 0, int y = 0)
      : xpos(x),
        ypos(y)
  {
  }

  void ShowPosition() const
  {
    cout << '[' << xpos << ", " << ypos << ']' << endl;
  }

  friend ostream &operator<<(ostream &os, const Point &);
  friend istream &operator>>(istream &is, Point &pos);

private:
  int xpos, ypos;
};

ostream &operator<<(ostream &os, const Point &pos)
{
  os << '[' << pos.xpos << ", " << pos.ypos << ']' << endl;
  return os;
}

istream &operator>>(istream &is, Point &pos)
{
  is >> pos.xpos >> pos.ypos;
  return is;
}

int main(void)
{
  Point pos1;
  cout << "x, y 순서로 입력 : ";
  cin >> pos1;
  cout << pos1;

  Point pos2;
  cout << "X, y 순서로 입력 : ";
  cin >> pos2;
  cout << pos2;

  return 0;
}
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

private:
  int xpos, ypos;
};

template <typename T>
void SwapData(T &lhs, T &rhs)
{
  T temp = lhs;
  lhs = rhs;
  rhs = temp;
}

int main(void)
{
  Point pos1(5, 1);
  Point pos2(1, 3);

  pos1.ShowPosition();
  pos2.ShowPosition();

  SwapData<Point>(pos1, pos2);

  pos1.ShowPosition();
  pos2.ShowPosition();
}
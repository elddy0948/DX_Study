#include <iostream>

using namespace std;

class Point
{
private:
  int xPos, yPos;

public:
  Point(int x = 0, int y = 0)
      : xPos(x),
        yPos(y)
  {
  }

  void ShowPosition() const
  {
    cout << '[' << xPos << ',' << yPos << ']' << endl;
  }

  Point &operator+=(const Point &pos)
  {
    xPos += pos.xPos;
    yPos += pos.yPos;
    return *this;
  }

  Point &operator-=(const Point &pos)
  {
    xPos -= pos.xPos;
    yPos -= pos.yPos;
    return *this;
  }

  Point &operator++()
  {
    xPos += 1;
    yPos += 1;
    return *this;
  }

  Point operator-()
  {
    Point pos(-xPos, -yPos);
    return pos;
  }

  const Point operator++(int)
  {
    const Point retobj(xPos, yPos);
    xPos += 1;
    yPos += 1;
    return retobj;
  }

  friend Point operator+(const Point &pos1, const Point &pos2);
  friend Point operator-(const Point &pos1, const Point &pos2);
  friend bool operator==(const Point &pos1, const Point &pos2);
  friend bool operator!=(const Point &pos1, const Point &pos2);
  friend Point &operator--(Point &ref);
  friend Point &operator~(Point &ref);
  friend const Point operator--(Point &ref, int);
};

Point operator+(const Point &pos1, const Point &pos2)
{
  Point pos(pos1.xPos + pos2.xPos, pos1.yPos + pos2.yPos);
  return pos;
}

Point operator-(const Point &pos1, const Point &pos2)
{
  Point pos(pos1.xPos - pos2.xPos, pos1.yPos - pos2.yPos);
  return pos;
}

bool operator==(const Point &pos1, const Point &pos2)
{
  if (pos1.xPos == pos2.xPos && pos1.yPos == pos2.yPos)
  {
    return true;
  }
  return false;
}

bool operator!=(const Point &pos1, const Point &pos2)
{
  if (!(pos1 == pos2))
  {
    return true;
  }
  return false;
}

Point &operator--(Point &ref)
{
  ref.xPos -= 1;
  ref.yPos -= 1;
  return ref;
}

Point &operator~(Point &ref)
{
  Point pos(ref.yPos, ref.xPos);
  return pos;
}

const Point operator--(Point &ref, int)
{
  const Point retobj(ref);
  ref.xPos -= 1;
  ref.yPos -= 1;
  return retobj;
}

int main()
{
  Point pos1(3, 4);
  Point pos2(10, 20);
  Point pos3 = pos1 + pos2;
  Point pos4 = pos2 + pos1;

  pos1.ShowPosition();
  pos2.ShowPosition();
  pos3.ShowPosition();
  pos4.ShowPosition();

  cout << (pos1 != pos4) << endl;

  return 0;
}
#ifndef __POINT_H_
#define __POINT_H_

class Point
{
private:
  int xpos;
  int ypos;

public:
  void Init(int x, int y);
  void ShowPointInfo() const;
};

#endif
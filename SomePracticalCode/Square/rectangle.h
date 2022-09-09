#pragma once

class Rectangle
{
public:
  Rectangle(int width, int height);
  ~Rectangle() = default;
  void ShowAreaInfo();

private:
  int width;
  int height;
};
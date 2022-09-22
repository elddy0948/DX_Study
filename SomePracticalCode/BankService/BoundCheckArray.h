#pragma once

#include <iostream>

using namespace std;

template <typename T>
class BoundCheckArray
{
private:
  T *arr;
  int arrLength;
  BoundCheckArray(const BoundCheckArray &arr) {}
  BoundCheckArray &operator=(const BoundCheckArray &arr) {}

public:
  BoundCheckArray(int length = 100);
  T &operator[](int index);
  T operator[](int index) const;
  int GetARrLength() const;
  ~BoundCheckArray();
};

template <typename T>
BoundCheckArray<T>::BoundCheckArray(int length) : arrLength(length)
{
  arr = new T[length];
}

template <typename T>
T &BoundCheckArray<T>::operator[](int index)
{
  if (index < 0 || index >= arrLength)
  {
    cout << "Array index out of bound exception" << endl;
    exit(1);
  }

  return arr[index];
}

template <typename T>
T BoundCheckArray<T>::operator[](int index) const
{
  if (index < 0 || index >= arrLength)
  {
    cout << "Array index out of bound exception" << endl;
    exit(1);
  }

  return arr[index];
}

template <typename T>
int BoundCheckArray<T>::GetARrLength() const
{
  return arrLength;
}

template <typename T>
BoundCheckArray<T>::~BoundCheckArray()
{
  delete[] arr;
}
#pragma once

#include <iostream>
#include <cstdlib>

using namespace std;

template <typename T>
class BoundCheckArray
{
public:
  BoundCheckArray(int length);
  T &operator[](int index);
  T operator[](int index) const;
  int GetArrLength() const;
  ~BoundCheckArray();

private:
  T *arr;
  int arrLength;
};

template <typename T>
BoundCheckArray<T>::BoundCheckArray(int length)
    : arrLength(length)
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
int BoundCheckArray<T>::GetArrLength() const
{
  return arrLength;
}

template <typename T>
BoundCheckArray<T>::~BoundCheckArray()
{
  delete[] arr;
}
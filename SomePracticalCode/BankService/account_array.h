#pragma once

#include <account.h>
typedef Account *ACCOUNT_PTR;

class BoundCheckAccountPtrArray
{
private:
  ACCOUNT_PTR *arr;
  int arrlength;
  BoundCheckAccountPtrArray(const BoundCheckAccountPtrArray &arr) {}
  BoundCheckAccountPtrArray &operator=(const BoundCheckAccountPtrArray &arr) {}

public:
  BoundCheckAccountPtrArray(int len = 100);
  ACCOUNT_PTR &operator[](int index);
  ACCOUNT_PTR operator[](int index) const;
  int GetArrayLength() const;
  ~BoundCheckAccountPtrArray();
};
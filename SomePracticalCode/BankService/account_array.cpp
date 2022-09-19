#include "BankingCommonDecl.h"
#include "account_array.h"

#include <iostream>

BoundCheckAccountPtrArray::BoundCheckAccountPtrArray(int len = 100)
{
  arr = new ACCOUNT_PTR[len];
}

ACCOUNT_PTR &BoundCheckAccountPtrArray::operator[](int index)
{
  if (index < 0 || index >= arrlength)
  {
    std::cout << "Array index out of bound exception" << std::endl;
    exit(1);
  }

  return arr[index];
}

ACCOUNT_PTR BoundCheckAccountPtrArray::operator[](int index) const
{
  if (index < 0 || index >= arrlength)
  {
    std::cout << "Array index out of bound exception" << std::endl;
    exit(1);
  }

  return arr[index];
}

int BoundCheckAccountPtrArray::GetArrayLength() const
{
  return arrlength;
}

BoundCheckAccountPtrArray::~BoundCheckAccountPtrArray()
{
  delete[] arr;
}
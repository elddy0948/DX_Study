#include "ebook.h"

#include <cstring>
#include <iostream>

EBook::EBook(const char *title, const char *isbn, int price, const char *DRMKey)
    : Book(title, isbn, price)
{
  this->DRMKey = new char[strlen(DRMKey) + 1];

  strcpy(this->DRMKey, DRMKey);
}

EBook::~EBook()
{
  delete[] DRMKey;
}

void EBook::ShowEBoookInfo()
{
  ShowBookInfo();
  std::cout << "DRMKey : " << DRMKey << std::endl;
}
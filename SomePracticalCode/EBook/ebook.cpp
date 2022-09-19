#include "ebook.h"

#include <cstring>
#include <iostream>

EBook::EBook(const char *title, const char *isbn, int price, const char *DRMKey)
    : Book(title, isbn, price)
{
  this->DRMKey = new char[strlen(DRMKey) + 1];

  strcpy(this->DRMKey, DRMKey);
}

EBook::EBook(const EBook &ref)
    : Book(ref)
{
  DRMKey = new char[strlen(ref.DRMKey) + 1];
  strcpy(DRMKey, ref.DRMKey);
}

EBook &EBook::operator=(const EBook &ref)
{
  Book::operator=(ref);
  delete[] DRMKey;

  DRMKey = new char[strlen(ref.DRMKey) + 1];
  strcpy(DRMKey, ref.DRMKey);

  return *this;
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
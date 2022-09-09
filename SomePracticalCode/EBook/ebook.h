#pragma once

#include "book.h"

class EBook : public Book
{
public:
  EBook(const char *title, const char *isbn, int price, const char *DRMKey);
  ~EBook();
  void ShowEBoookInfo();

private:
  char *DRMKey;
};
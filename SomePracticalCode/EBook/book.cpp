#include "book.h"

#include <cstring>
#include <iostream>

Book::Book(const char *title, const char *isbn, int price)
    : price(price)
{
  this->title = new char[strlen(title) + 1];
  this->isbn = new char[strlen(isbn) + 1];

  strcpy(this->title, title);
  strcpy(this->isbn, isbn);
}

Book::Book(const Book &ref)
    : price(ref.price)
{
  title = new char[strlen(ref.title) + 1];
  isbn = new char[strlen(ref.isbn) + 1];

  strcpy(title, ref.title);
  strcpy(isbn, ref.isbn);
}

Book &Book::operator=(const Book &ref)
{
  delete[] title;
  delete[] isbn;

  title = new char[strlen(ref.title) + 1];
  isbn = new char[strlen(ref.isbn) + 1];

  strcpy(title, ref.title);
  strcpy(isbn, ref.isbn);
  price = ref.price;

  return *this;
}

Book::~Book()
{
  delete[] title;
  delete[] isbn;
}

void Book::ShowBookInfo()
{
  std::cout << "Title : " << title << std::endl;
  std::cout << "ISBN : " << isbn << std::endl;
  std::cout << "Price : " << price << std::endl;
}
#pragma once

class Book
{
public:
  Book(const char *title, const char *isbn, int price);
  Book(const Book &ref);
  Book &operator=(const Book &ref);
  ~Book();
  void ShowBookInfo();

private:
  char *title;
  char *isbn;
  int price;
};
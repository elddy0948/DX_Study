#pragma once

class Book
{
public:
  Book(const char *title, const char *isbn, int price);
  ~Book();
  void ShowBookInfo();

private:
  char *title;
  char *isbn;
  int price;
};
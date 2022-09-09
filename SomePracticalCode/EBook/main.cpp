#include "ebook.h"

#include <iostream>
#include <cstring>

int main(void)
{
  Book book("Good C++", "555-12345-890-0", 20000);
  book.ShowBookInfo();
  std::cout << std::endl;

  EBook ebook("Good C++ EBook", "555-12345-890-1", 10000, "fdx9w0u8kiw");
  ebook.ShowEBoookInfo();

  return 0;
}
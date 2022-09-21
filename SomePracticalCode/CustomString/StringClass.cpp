#include "StringClass.h"

String::String()
{
  length = 0;
  str = nullptr;
}

String::String(const char *s)
{
  length = strlen(s) + 1;
  str = new char[length];
  strcpy(str, s);
}

String::String(const String &s)
{
  length = s.length;
  str = new char[length];
  strcpy(str, s.str);
}

String::~String()
{
  if (str != nullptr)
  {
    delete[] str;
  }
}

String &String::operator=(const String &s)
{
  if (str != nullptr)
  {
    delete[] str;
  }
  length = s.length;
  str = new char[length];
  strcpy(str, s.str);
  return *this;
}

String &String::operator+=(const String &s)
{
  length += (s.length - 1);
  char *tempstr = new char[length];

  strcpy(tempstr, str);
  strcat(tempstr, s.str);

  if (str != NULL)
  {
    delete[] str;
  }

  str = tempstr;
  return *this;
}

bool String::operator==(const String &s)
{
  return strcmp(str, s.str) ? false : true;
}

String String::operator+(const String &s)
{
  char *tempstr = new char[length + s.length + 1];
  strcpy(tempstr, str);
  strcat(tempstr, s.str);

  String temp(tempstr);
  delete[] tempstr;

  return temp;
}

std::ostream &operator<<(std::ostream &os, const String &s)
{
  os << s.str;
  return os;
}

std::istream &operator>>(std::istream &is, String &s)
{
  char str[100];
  is >> str;
  s = String(str);
  return is;
}

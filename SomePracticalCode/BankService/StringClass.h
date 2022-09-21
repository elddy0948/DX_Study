#pragma once

#include "BankingCommonDecl.h"

#include <iostream>
#include <cstring>

class String
{
public:
  String();
  String(const char *s);
  String(const String &s);
  ~String();

  String &operator=(const String &s);
  String &operator+=(const String &s);
  bool operator==(const String &s);
  String operator+(const String &s);

  friend std::ostream &operator<<(std::ostream &os, const String &s);
  friend std::istream &operator>>(std::istream &is, String &s);

private:
  int length;
  char *str;
};
#include <iostream>
#include <cstring>

using namespace std;

template <typename T>
T Max(T lhs, T rhs)
{
  return lhs > rhs ? lhs : rhs;
}

template <>
char *Max(char *lhs, char *rhs)
{
  cout << "Char* Max<char*>(char* lhs, char* rhs)" << endl;
  return strlen(lhs) > strlen(rhs) ? lhs : rhs;
}

template <>
const char *Max(const char *lhs, const char *rhs)
{
  cout << "const char* Max<const char*>(const char* lhs, const char* rhs)" << endl;
  return strcmp(lhs, rhs) > 0 ? lhs : rhs;
}

int main(void)
{
  cout << Max(11, 15) << endl;
  cout << Max('T', 'Q') << endl;
  cout << Max("Simple", "Best") << endl;

  char str1[] = "Simple";
  char str2[] = "Best";

  cout << Max(str1, str2);

  return 0;
}
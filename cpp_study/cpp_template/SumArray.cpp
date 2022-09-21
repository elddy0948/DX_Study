#include <iostream>

using namespace std;

template <typename T>
T SumArray(T arr[], int len)
{
  T sum;

  for (int i = 0; i < len; i++)
  {
    sum += arr[i];
  }

  return sum;
}

int main(void)
{
  double array[5] = {1.1, 2.2, 3.3, 4.4, 5.5};
  double sum = SumArray<double>(array, 5);

  cout << sum << endl;

  return 0;
}
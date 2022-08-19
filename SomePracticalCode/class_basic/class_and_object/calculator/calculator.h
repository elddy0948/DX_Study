#ifndef CALCULATOR
#define CALCULATOR

class Calculator
{
private:
  int plusCounter;
  int minusCounter;
  int multiplicationCounter;
  int dividerCounter;

public:
  void Init();
  double Add(const double lhs, const double rhs);
  double Min(const double lhs, const double rhs);
  double Mul(const double lhs, const double rhs);
  double Div(const double lhs, const double rhs);
  void ShowCalculatorState();
};

#endif
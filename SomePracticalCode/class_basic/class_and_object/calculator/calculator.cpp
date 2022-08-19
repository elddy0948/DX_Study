#include "calculator.h"
#include <iostream>

void Calculator::Init()
{
  plusCounter = 0;
  minusCounter = 0;
  multiplicationCounter = 0;
  dividerCounter = 0;
}

double Calculator::Add(const double lhs, const double rhs)
{
  plusCounter++;
  return lhs + rhs;
}
double Calculator::Min(const double lhs, const double rhs)
{
  minusCounter++;
  return lhs - rhs;
}
double Calculator::Mul(const double lhs, const double rhs)
{
  multiplicationCounter++;
  return lhs * rhs;
}
double Calculator::Div(const double lhs, const double rhs)
{
  dividerCounter++;
  return lhs / rhs;
}

void Calculator::ShowCalculatorState()
{
  std::cout << "Add : " << plusCounter << " Minus : " << minusCounter << " Multi : " << multiplicationCounter << " Div : " << dividerCounter << std::endl;
}
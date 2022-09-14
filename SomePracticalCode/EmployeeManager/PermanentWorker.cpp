#include "PermanentWorker.h"

#include <iostream>

PermanentWorker::PermanentWorker(char *name, int money)
    : Employee(name),
      salary(money)
{
}

int PermanentWorker::GetPay() const
{
  return salary;
}

void PermanentWorker::ShowSalaryInfo() const
{
  ShowYourName();
  std::cout << "Salary : " << GetPay() << std::endl
            << std::endl;
}
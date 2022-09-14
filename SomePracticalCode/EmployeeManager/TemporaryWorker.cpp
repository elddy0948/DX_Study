#include "TemporaryWorker.h"

#include <iostream>

TemporaryWorker::TemporaryWorker(char *name, int pay)
    : Employee(name),
      workTime(0),
      payPerHour(pay)
{
}

void TemporaryWorker::AddWorkTime(int time)
{
  workTime += time;
}

int TemporaryWorker::GetPay() const
{
  return workTime * payPerHour;
}

void TemporaryWorker::ShowSalaryInfo() const
{
  ShowYourName();
  std::cout << "Salary : " << GetPay() << std::endl
            << std::endl;
}
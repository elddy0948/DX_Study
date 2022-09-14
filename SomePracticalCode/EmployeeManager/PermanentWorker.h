#pragma once

#include "Employee.h"

class PermanentWorker : public Employee
{
private:
  int salary;

public:
  PermanentWorker(char *name, int money);
  int GetPay() const;
  void ShowSalaryInfo() const;
};
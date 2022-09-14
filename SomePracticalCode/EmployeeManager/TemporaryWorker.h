#pragma once

#include "Employee.h"

class TemporaryWorker : public Employee
{
public:
  TemporaryWorker(char *name, int pay);
  void AddWorkTime(int time);
  int GetPay() const;
  void ShowSalaryInfo() const;

private:
  int workTime;
  int payPerHour;
};
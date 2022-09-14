#pragma once

#include "PermanentWorker.h"

class SalesWorker : public PermanentWorker
{
public:
  SalesWorker(char *name, int money, double ratio);
  void AddSalesResult(int value);
  int GetPay() const;
  void ShowSalaryInfo() const;

private:
  int salesResult;
  double bonusRatio;
};
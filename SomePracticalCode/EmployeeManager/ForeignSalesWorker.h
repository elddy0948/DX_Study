#pragma once

#include "SalesWorker.h"

namespace RISK_LEVEL
{
  enum
  {
    RISK_A,
    RISK_B,
    RISK_C
  };
}

class ForeignSalesWorker : public SalesWorker
{
public:
  ForeignSalesWorker(
      char *name,
      int money,
      double ratio,
      int riskLevel);
  int GetPay() const;
  void ShowSalaryInfo() const;

private:
  int riskLevel;
};
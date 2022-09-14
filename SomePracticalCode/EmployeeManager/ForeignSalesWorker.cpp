#include "ForeignSalesWorker.h"

#include <iostream>

ForeignSalesWorker::ForeignSalesWorker(
    char *name,
    int money,
    double ratio,
    int riskLevel)
    : SalesWorker(name, money, ratio),
      riskLevel(riskLevel)
{
}

int ForeignSalesWorker::GetPay() const
{
  double insentive;

  switch (riskLevel)
  {
  case RISK_LEVEL::RISK_A:
    insentive = 0.3;
    break;
  case RISK_LEVEL::RISK_B:
    insentive = 0.2;
    break;
  case RISK_LEVEL::RISK_C:
    insentive = 0.1;
    break;
  default:
    insentive = 0.0;
    break;
  }

  int basePay = SalesWorker::GetPay();
  int total = basePay + (basePay * insentive);

  return total;
}

void ForeignSalesWorker::ShowSalaryInfo() const
{
  ShowYourName();
  std::cout << "Salary : " << GetPay() << std::endl
            << std::endl;
}
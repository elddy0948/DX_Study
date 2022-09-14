#include "PermanentWorker.h"
#include "TemporaryWorker.h"
#include "SalesWorker.h"
#include "EmployeeHandler.h"
#include "ForeignSalesWorker.h"

#include <iostream>
#include <cstring>

using namespace std;

int main(void)
{
  EmployeeHandler handler;

  handler.AddEmployee(new PermanentWorker("KIM", 1000));
  handler.AddEmployee(new PermanentWorker("LEE", 1500));

  TemporaryWorker *alba = new TemporaryWorker("JUNG", 700);
  alba->AddWorkTime(5);
  handler.AddEmployee(alba);

  ForeignSalesWorker *fseller1 = new ForeignSalesWorker("Hong", 1000, 0.1, RISK_LEVEL::RISK_A);
  fseller1->AddSalesResult(7000);
  handler.AddEmployee(fseller1);

  ForeignSalesWorker *fseller2 = new ForeignSalesWorker("Yoon", 1000, 0.1, RISK_LEVEL::RISK_B);
  fseller2->AddSalesResult(7000);
  handler.AddEmployee(fseller2);

  handler.ShowAllSalaryInfo();

  handler.ShowTotalSalary();

  return 0;
}
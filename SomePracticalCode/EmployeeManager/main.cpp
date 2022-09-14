#include "PermanentWorker.h"
#include "TemporaryWorker.h"
#include "SalesWorker.h"
#include "EmployeeHandler.h"

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

  SalesWorker *seller = new SalesWorker("Hong", 1000, 0.1);
  seller->AddSalesResult(7000);
  handler.AddEmployee(seller);

  handler.ShowAllSalaryInfo();

  handler.ShowTotalSalary();

  return 0;
}
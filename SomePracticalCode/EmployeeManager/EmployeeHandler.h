#pragma once

#include "PermanentWorker.h"
#include "TemporaryWorker.h"
#include "SalesWorker.h"

class EmployeeHandler
{
private:
  Employee *employeeList[50];
  int employeeNumber;

public:
  EmployeeHandler();
  void AddEmployee(Employee *emp);
  void ShowAllSalaryInfo() const;
  void ShowTotalSalary() const;
  ~EmployeeHandler();
};
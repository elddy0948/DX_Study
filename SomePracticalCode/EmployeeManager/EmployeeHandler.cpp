#include "EmployeeHandler.h"

#include <iostream>

EmployeeHandler::EmployeeHandler()
    : employeeNumber(0)
{
}

void EmployeeHandler::AddEmployee(Employee *emp)
{
  employeeList[employeeNumber++] = emp;
}

void EmployeeHandler::ShowAllSalaryInfo() const
{
  for (int i = 0; i < employeeNumber; i++)
  {
    employeeList[i]->ShowSalaryInfo();
  }
}

void EmployeeHandler::ShowTotalSalary() const
{
  int sum = 0;
  for (int i = 0; i < employeeNumber; i++)
  {
    sum += employeeList[i]->GetPay();
  }
  std::cout << "Salary Sum : " << sum << std::endl;
}

EmployeeHandler::~EmployeeHandler()
{
  for (int i = 0; i < employeeNumber; i++)
  {
    delete employeeList[i];
  }
}
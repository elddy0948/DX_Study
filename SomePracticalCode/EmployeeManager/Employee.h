#pragma once

class Employee
{
private:
  char name[100];

public:
  Employee(char *name);
  virtual int GetPay() const = 0;
  virtual void ShowSalaryInfo() const = 0;
  void ShowYourName() const;
};
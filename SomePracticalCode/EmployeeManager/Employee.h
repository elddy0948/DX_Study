#pragma once

class Employee
{
private:
  char name[100];

public:
  Employee(char *name);
  virtual int GetPay() const;
  virtual void ShowSalaryInfo() const;
  void ShowYourName() const;
};
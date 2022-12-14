#pragma once

#include "StringClass.h"

class Account
{
public:
  Account(int id, int money, String name);
  // Account(int id, int money, char *name);
  // Account(const Account &ref);
  // Account &operator=(const Account &ref);
  // ~Account();

  int GetAccountID() const;
  virtual void Deposit(int money);
  int Withdraw(int money);
  void ShowAccountInfo() const;

private:
  int accountID;
  int balance;
  // char *customerName;
  String customerName;
};
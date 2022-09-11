#pragma once

#include "account.h"

class AccountHandler
{
public:
  AccountHandler();
  ~AccountHandler();

  void ShowMenu(void) const;
  void MakeAccount(void);
  void DepositMoney(void);
  void WithdrawMoney(void);
  void ShowAllAccountInfo(void) const;

private:
  Account *accountArray[100];
  int accountNumber;
};
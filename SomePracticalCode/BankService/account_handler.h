#pragma once

#include "account.h"
#include "BoundCheckArray.h"

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

protected:
  void MakeNormalAccount(void);
  void MakeCreditAccount(void);

private:
  BoundCheckArray<Account *> accountArray;
  int accountNumber;
};
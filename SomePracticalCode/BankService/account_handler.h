#pragma once

#include "account.h"
#include "account_array.h"

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
  BoundCheckAccountPtrArray accountArray;
  int accountNumber;
};
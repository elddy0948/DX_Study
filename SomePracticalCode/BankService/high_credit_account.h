#pragma once

#include "normal_account.h"

class HighCreditAccount : public NormalAccount
{
public:
  HighCreditAccount(
      int ID, int money, char *name, int rate, int special);
  virtual void Deposit(int money);

private:
  int specialRate;
};
#pragma once

#include "normal_account.h"
#include "StringClass.h"

class HighCreditAccount : public NormalAccount
{
public:
  HighCreditAccount(
      int ID, int money, String name, int rate, int special);
  virtual void Deposit(int money);

private:
  int specialRate;
};
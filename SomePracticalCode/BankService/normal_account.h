#pragma once

#include "account.h"

class NormalAccount : public Account
{
public:
  NormalAccount(int ID, int money, char *name, int rate);
  virtual void Deposit(int money);

private:
  int interRate; // 이자율 %단위
};
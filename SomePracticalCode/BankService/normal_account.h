#pragma once

#include "account.h"
#include "StringClass.h"

class NormalAccount : public Account
{
public:
  // NormalAccount(int ID, int money, char *name, int rate);
  NormalAccount(int ID, int money, String name, int rate);
  virtual void Deposit(int money);

private:
  int interRate; // 이자율 %단위
};
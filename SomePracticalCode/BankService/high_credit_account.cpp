#include "high_credit_account.h"

HighCreditAccount::HighCreditAccount(
    int ID, int money, String name, int rate, int special)
    : NormalAccount(ID, money, name, rate),
      specialRate(special)
{
}

void HighCreditAccount::Deposit(int money)
{
  NormalAccount::Deposit(money);
  Account::Deposit(money * (specialRate / 100.0));
}
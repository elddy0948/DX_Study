#include "account.h"

#include <iostream>
#include <cstring>

Account::Account(int id, int money, char *name)
    : accountID(id),
      balance(money)
{
  customerName = new char[strlen(name) + 1];
  strcpy(customerName, name);
}

Account::Account(const Account &ref)
    : accountID(ref.accountID),
      balance(ref.balance)
{
  customerName = new char[strlen(ref.customerName) + 1];
  strcpy(customerName, ref.customerName);
}

Account::~Account()
{
  delete[] customerName;
}

int Account::GetAccountID() const
{
  return accountID;
}

void Account::Deposit(int money)
{
  balance += money;
}

int Account::Withdraw(int money)
{
  if (balance < money)
  {
    return 0;
  }
  balance -= money;
  return money;
}

void Account::ShowAccountInfo() const
{
  std::cout << "Account ID : " << accountID << std::endl;
  std::cout << "Name : " << customerName << std::endl;
  std::cout << "Balance : " << balance << std::endl;
}
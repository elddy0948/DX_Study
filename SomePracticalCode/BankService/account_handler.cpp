#include "account_handler.h"

#include <iostream>
#include <cstring>

const int NAME_LEN = 20;

AccountHandler::AccountHandler()
    : accountNumber(0)
{
}

AccountHandler::~AccountHandler()
{
  for (int i = 0; i < accountNumber; i++)
  {
    delete accountArray[i];
  }
}

void AccountHandler::ShowMenu(void) const
{
  std::cout << "-----Menu-----" << std::endl;
  std::cout << "1. Create Account" << std::endl;
  std::cout << "2. Deposit" << std::endl;
  std::cout << "3. Withdraw" << std::endl;
  std::cout << "4. Show All Account Info" << std::endl;
  std::cout << "5. Exit" << std::endl;
}

void AccountHandler::MakeAccount(void)
{
  int id;
  char name[NAME_LEN];
  int balance;

  std::cout << "[Create Account]" << std::endl;
  std::cout << "Account ID : ";
  std::cin >> id;
  std::cout << "Name : ";
  std::cin >> name;
  std::cout << std::endl;

  accountArray[accountNumber++] = new Account(id, balance, name);
}

void AccountHandler::DepositMoney(void)
{
  int money;
  int id;

  std::cout << "[Deposit]" << std::endl;
  std::cout << "Account ID : ";
  std::cin >> id;
  std::cout << "How much : ";
  std::cin >> money;

  for (int i = 0; i < accountNumber; i++)
  {
    if (accountArray[i]->GetAccountID() == id)
    {
      accountArray[i]->Deposit(money);
      std::cout << "Done!" << std::endl;
      return;
    }
  }

  std::cout << "Invalid Account ID" << std::endl
            << std::endl;
}

void AccountHandler::WithdrawMoney(void)
{
  int money;
  int id;

  std::cout << "[Withdraw]" << std::endl;
  std::cout << "Account ID : " << std::endl;
  std::cout << "How much : " << std::endl;

  for (int i = 0; i < accountNumber; i++)
  {
    if (accountArray[i]->GetAccountID() == id)
    {
      if (accountArray[i]->Withdraw(money) == 0)
      {
        std::cout << "No Money... :(" << std::endl
                  << std::endl;
      }
      std::cout << "Done!" << std::endl;
      return;
    }
  }
  std::cout << "Invalid ID" << std::endl
            << std::endl;
}

void AccountHandler::ShowAllAccountInfo(void) const
{
}
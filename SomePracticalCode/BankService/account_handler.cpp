#include "account_handler.h"
#include "normal_account.h"
#include "high_credit_account.h"

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
  int sel;

  std::cout << "[Select Account Type]" << std::endl;
  std::cout << "1. Normal Account" << std::endl;
  std::cout << "2. Credit Account" << std::endl;
  std::cout << "Select : ";
  std::cin >> sel;

  if (sel == ACCOUNT_CONST::NORMAL)
  {
    MakeNormalAccount();
  }
  else
  {
    MakeCreditAccount();
  }
}

void AccountHandler::MakeNormalAccount()
{
  int id;
  char name[NAME_LEN];
  int balance;
  int interRate;

  std::cout << "[Create Normal Account]" << std::endl;
  std::cout << "Account ID : ";
  std::cin >> id;
  std::cout << "Name : ";
  std::cin >> name;
  std::cout << std::endl;
  std::cout << "Balance : ";
  std::cin >> balance;
  std::cout << "Inter Rate : ";
  std::cin >> interRate;
  std::cout << std::endl;

  accountArray[accountNumber++] = new NormalAccount(id, balance, name, interRate);
}

void AccountHandler::MakeCreditAccount()
{
  int id;
  char name[NAME_LEN];
  int balance;
  int interRate;
  int creditLevel;

  std::cout << "[Create Credit Account]" << std::endl;
  std::cout << "Account ID : ";
  std::cin >> id;
  std::cout << "Name : ";
  std::cin >> name;
  std::cout << std::endl;
  std::cout << "Balance : ";
  std::cin >> balance;
  std::cout << "Inter Rate : ";
  std::cin >> interRate;
  std::cout << "Credit Level(1 to A, 2 to B, 3 to C) : ";
  std::cin >> creditLevel;
  std::cout << std::endl;

  switch (creditLevel)
  {
  case 1:
    accountArray[accountNumber++] = new HighCreditAccount(id, balance, name, interRate, LEVEL_CONST::LEVEL_A);
    break;
  case 2:
    accountArray[accountNumber++] = new HighCreditAccount(id, balance, name, interRate, LEVEL_CONST::LEVEL_B);
    break;
  case 3:
    accountArray[accountNumber++] = new HighCreditAccount(id, balance, name, interRate, LEVEL_CONST::LEVEL_C);
    break;
  }
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
  for (int i = 0; i < accountNumber; i++)
  {
    accountArray[i]->ShowAccountInfo();
    std::cout << std::endl;
  }
}
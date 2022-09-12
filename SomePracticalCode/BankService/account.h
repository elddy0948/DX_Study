#pragma once

/*
  Account
  Entity Class
*/

class Account
{
public:
  Account(int id, int money, char *name);
  Account(const Account &ref);
  ~Account();

  int GetAccountID() const;
  void Deposit(int money);
  int Withdraw(int money);
  void ShowAccountInfo() const;

private:
  int accountID;
  int balance;
  char *customerName;
};

namespace MENU_CONST
{
  enum
  {
    MAKE = 1,
    DEPOSIT,
    WITHDRAW,
    INQUIRE,
    EXIT
  };
};
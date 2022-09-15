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
  virtual void Deposit(int money);
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

namespace LEVEL_CONST
{
  enum
  {
    LEVEL_A = 7,
    LEVEL_B = 4,
    LEVEL_C = 2
  };
};

namespace ACCOUNT_CONST
{
  enum
  {
    NORMAL = 1,
    CREDIT = 2
  };
};
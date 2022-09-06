#include <iostream>
#include <cstring>

using namespace std;

class Account
{
private:
  int accountID;
  int balance;
  char *customerName;

public:
  Account(int ID, int money, char *name)
      : accountID(ID),
        balance(money)
  {
    customerName = new char[strlen(name) + 1];
    strcpy(customerName, name);
  }

  Account(const Account &ref)
      : accountID(ref.accountID),
        balance(ref.balance)
  {
    customerName = new char[strlen(ref.customerName) + 1];
    strcpy(customerName, ref.customerName);
  }

  int GetAccountID() const
  {
    return accountID;
  }

  void Deposit(int money)
  {
    balance += money;
  }

  int Withdraw(int money)
  {
    if (balance < money)
    {
      return 0;
    }
    balance -= money;
    return money;
  }

  void ShowAccountInfo() const
  {
    cout << "Account ID : " << accountID << endl;
    cout << "Name : " << customerName << endl;
    cout << "Balance : " << balance << endl;
  }

  ~Account()
  {
    delete[] customerName;
  }
};

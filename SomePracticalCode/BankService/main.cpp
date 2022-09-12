#include "account_handler.h"
#include "account.h"

#include <iostream>

int main(void)
{
  AccountHandler manager;
  int choice;

  while (1)
  {
    manager.ShowMenu();
    std::cout << "Choice : ";
    std::cin >> choice;
    std::cout << std::endl;

    switch (choice)
    {
    case MENU_CONST::MAKE:
      manager.MakeAccount();
      break;
    case MENU_CONST::DEPOSIT:
      manager.DepositMoney();
      break;
    case MENU_CONST::WITHDRAW:
      manager.WithdrawMoney();
      break;
    case MENU_CONST::INQUIRE:
      manager.ShowAllAccountInfo();
      break;
    case MENU_CONST::EXIT:
      return 0;
    default:
      std::cout << "Illegal selection.." << std::endl;
    }
  }

  return 0;
}
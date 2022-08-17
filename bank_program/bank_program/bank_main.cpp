#include "bank_program.h"

#include <iostream>

int main(void)
{
    bank::Bank bank;

    while (1)
    {
        std::cout << "---- Menu ----" << std::endl;
        std::cout << "1. Make Account" << std::endl;
        std::cout << "2. Deposit" << std::endl;
        std::cout << "3. Withdraw" << std::endl;
        std::cout << "4. Show all account info" << std::endl;
        std::cout << "5. Exit" << std::endl;

        int task;
        std::cin >> task;

        switch (task)
        {
        case 1:
            bank.MakeAccount();
            break;
        case 2:
            bank.Deposit();
            break;
        case 3:
            bank.Withdraw();
            break;
        case 4:
            bank.ShowAllAccountInfo();
            break;
        default:
            break;
        }
    }
    return 0;
}
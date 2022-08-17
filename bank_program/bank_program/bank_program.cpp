#include "bank_program.h"
#include <iostream>


namespace bank
{
    void Bank::MakeAccount()
    {
        std::cout << "[���� ����]" << std::endl;

        Account account;

        std::cout << "Account ID : ";
        std::cin >> account.accountID;
        std::cout << "Name : ";
        std::cin >> account.customerName;
        std::cout << "Money : ";
        std::cin >> account.money;

        accounts[accountCounter] = account;
        accountCounter++;

        return;
    }

    void Bank::Deposit()
    {
        int id;
        int money;

        std::cout << "�� ��" << std::endl;
        std::cout << "Account ID : ";
        std::cin >> id;
        std::cout << "�Աݾ�";
        std::cin >> money;

        for (int i = 0; i < accountCounter; i++)
        {
            if (accounts[i].accountID == id)
            {
                accounts[i].money += money;
                std::cout << "�Ա� �Ϸ�!" << std::endl;
            }
        }

        return;
    }
    void Bank::Withdraw()
    {
        int id;
        int money;

        std::cout << "�� ��" << std::endl;
        std::cout << "Account ID : ";
        std::cin >> id;
        std::cout << "Money : ";
        std::cin >> money;

        for (int i = 0; i < accountCounter; i++)
        {
            if (accounts[i].accountID == id)
            {
                if (accounts[i].money < money)
                {
                    std::cout << "�ܾ� ����!" << std::endl;
                    return;
                }
                accounts[i].money -= money;
                std::cout << "��� �Ϸ�" << std::endl;
            }
        }
    }
    void Bank::ShowAllAccountInfo()
    {
        for (int i = 0; i < accountCounter; i++)
        {
            std::cout << "Account ID : " << accounts[i].accountID << std::endl;
            std::cout << "Money : " << accounts[i].money << std::endl;
            std::cout << "Name : " << accounts[i].customerName << std::endl;
        }
    }
}
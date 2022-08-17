#pragma once

namespace bank
{
    typedef struct
    {
        int accountID;
        int money;
        char customerName[20];
    } Account;

    class Bank
    {
    public:
        void MakeAccount();
        void Deposit();
        void Withdraw();
        void ShowAllAccountInfo();

    private:
        Account accounts[100];
        int accountCounter = 0;
    };
}
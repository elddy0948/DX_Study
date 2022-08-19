#include "fruit_buyer.h"

#include <iostream>

void FruitBuyer::InitMembers(int money)
{
  myMoney = money;
  numOfApples = 0;
}
void FruitBuyer::BuyApples(FruitSeller &seller, int money)
{
  numOfApples = seller.SaleApples(money);
  myMoney -= money;
}
void FruitBuyer::ShowBuyResult()
{
  std::cout << "Remain Money : " << myMoney << std::endl;
  std::cout << "Remain Apples : " << numOfApples << std::endl;
}
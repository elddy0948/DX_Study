#include "fruit_buyer.h"

#include <iostream>

FruitBuyer::FruitBuyer(int money)
{
  myMoney = money;
  numOfApples = 0;
  return;
}

void FruitBuyer::BuyApples(FruitSeller &seller, int money)
{
  if (0 > money)
  {
    std::cout << "money 는 0보다 커야합니다." << std::endl;
    return;
  }
  numOfApples = seller.SaleApples(money);
  myMoney -= money;
  return;
}
void FruitBuyer::ShowBuyResult() const
{
  std::cout << "Remain Money : " << myMoney << std::endl;
  std::cout << "Remain Apples : " << numOfApples << std::endl;
}
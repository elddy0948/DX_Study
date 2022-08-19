#include "fruit_seller.h"

#include <iostream>

void FruitSeller::InitMembers(int price, int num, int money)
{
  APPLE_PRICE = price;
  numOfApples = num;
  myMoney = money;
}
int FruitSeller::SaleApples(int money)
{
  int num = money / APPLE_PRICE;
  numOfApples -= num;
  myMoney += money;
  return num;
}
void FruitSeller::ShowSalesResult()
{
  std::cout << "Remain Apples : " << numOfApples << std::endl;
  std::cout << "Money : " << myMoney << std::endl;
}
#include "fruit_seller.h"

#include <iostream>

FruitSeller::FruitSeller(int price, int num, int money)
{
  APPLE_PRICE = price;
  numOfApples = num;
  myMoney = money;
}

int FruitSeller::SaleApples(int money)
{
  if (money < 0)
  {
    std::cout << "잘못된 정보가 전달되어 판매를 취소합니다." << std::endl;
    return 0;
  }
  int num = money / APPLE_PRICE;
  numOfApples -= num;
  myMoney += money;
  return num;
}
void FruitSeller::ShowSalesResult() const
{
  std::cout << "Remain Apples : " << numOfApples << std::endl;
  std::cout << "Money : " << myMoney << std::endl;
}
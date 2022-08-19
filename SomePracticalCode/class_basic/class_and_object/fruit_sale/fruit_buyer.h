#ifndef FRUITBUYER
#define FRUITBUYER

#include "fruit_seller.h"

class FruitBuyer
{
private:
  int myMoney;
  int numOfApples;

public:
  void InitMembers(int money);
  void BuyApples(FruitSeller &seller, int money);
  void ShowBuyResult();
};

#endif
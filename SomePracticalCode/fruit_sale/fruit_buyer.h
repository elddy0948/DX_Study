#ifndef FRUITBUYER
#define FRUITBUYER

#include "fruit_seller.h"

class FruitBuyer
{
private:
  int myMoney;
  int numOfApples;

public:
  FruitBuyer(int money);
  void BuyApples(FruitSeller &seller, int money);
  void ShowBuyResult() const;
};

#endif
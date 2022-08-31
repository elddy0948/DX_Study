#include "fruit_seller.h"
#include "fruit_buyer.h"

#include <iostream>

int main(void)
{
  FruitSeller seller(1000, 20, 0);

  FruitBuyer buyer(5000);
  buyer.BuyApples(seller, 2000);

  std::cout << "seller info : " << std::endl;
  seller.ShowSalesResult();

  std::cout << "buyer info : " << std::endl;
  buyer.ShowBuyResult();

  return 0;
}
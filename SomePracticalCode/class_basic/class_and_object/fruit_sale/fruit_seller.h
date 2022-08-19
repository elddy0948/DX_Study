#ifndef FRUITSELLER
#define FRUITSELLER

class FruitSeller
{
private:
  int APPLE_PRICE;
  int numOfApples;
  int myMoney;

public:
  void InitMembers(int price, int num, int money);
  int SaleApples(int money);
  void ShowSalesResult();
};

#endif
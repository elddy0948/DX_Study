#include <iostream>

using namespace std;

namespace CAR_CONST
{
  enum
  {
    ID_LEN = 20,
    MAX_SPD = 200,
    FUEL_STEP = 2,
    ACC_STEP = 10,
    BRK_STEP = 10
  };
}

struct Car
{
  char gamerID[CAR_CONST::ID_LEN];
  int fuelGauge;
  int currentSpeed;

  // Function
  void ShowCarState();
  void Accel();
  void Break();
};

void Car::ShowCarState()
{
  cout << "Owner ID : " << gamerID << endl;
  cout << "Fuel : " << fuelGauge << "%" << endl;
  cout << "Current Speed : " << currentSpeed << "km/s" << endl;
}

void Car::Accel()
{
  if (fuelGauge <= 0)
  {
    return;
  }
  else
  {
    fuelGauge -= CAR_CONST::FUEL_STEP;
  }

  if ((currentSpeed + CAR_CONST::ACC_STEP) >= CAR_CONST::MAX_SPD)
  {
    currentSpeed = CAR_CONST::MAX_SPD;
    return;
  }
  currentSpeed += CAR_CONST::ACC_STEP;
}

void Car::Break()
{
  if (currentSpeed < CAR_CONST::BRK_STEP)
  {
    currentSpeed = 0;
    return;
  }

  currentSpeed -= CAR_CONST::BRK_STEP;
}

int main(void)
{
  Car run99 = {"run99", 100, 0};
  run99.Accel();
  run99.ShowCarState();
  run99.Break();
  run99.ShowCarState();
  return 0;
}
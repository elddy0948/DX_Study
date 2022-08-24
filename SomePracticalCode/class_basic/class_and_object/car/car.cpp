#include <iostream>
#include <cstring>
#include "car.h"

using namespace std;

void Car::InitMembers(char *ID, int fuel)
{
  strcpy(gamerID, ID);
  fuelGauge = fuel;
  currentSpeed = 0;
}
void Car::ShowCarState()
{
  cout << "Owner : " << gamerID << endl;
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

  if ((currentSpeed + CAR_CONST::ACC_STEP >= CAR_CONST::MAX_SPD))
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

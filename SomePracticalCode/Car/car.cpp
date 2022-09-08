#include "car.h"

#include <iostream>

using namespace std;

// Car Stuff
Car::Car(int gauge)
    : gasolineGauge(gauge)
{
}

int Car::GetGasGauge()
{
  return gasolineGauge;
}

HybridCar::HybridCar(int gas, int elec)
    : Car(gas),
      electricGauge(elec)
{
}

int HybridCar::GetElecGauge()
{
  return electricGauge;
}

HybridWaterCar::HybridWaterCar(int gas, int elec, int water)
    : HybridCar(gas, elec),
      waterGauge(water)
{
}

void HybridWaterCar::ShowCurrentGauge()
{
  cout << "Remain Gas : " << GetGasGauge() << endl;
  cout << "Remain Elec : " << GetElecGauge() << endl;
  cout << "Remain Water : " << waterGauge << endl;
}

int main(void)
{
  HybridWaterCar myCar(300, 200, 100);

  myCar.ShowCurrentGauge();
}
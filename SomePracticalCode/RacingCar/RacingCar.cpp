#include <iostream>

using namespace std;

#define ID_LEN 20
#define MAX_SPD 200
#define FUEL_STEP 2
#define ACC_STEP 10
#define BRK_STEP 10

struct Car
{
  char gamerID[ID_LEN];
  int fuelGauge;
  int currentSpeed;

  void ShowCarState()
  {
    cout << "Owner ID : " << gamerID << endl;
    cout << "Fuel : " << fuelGauge << " % " << endl;
    cout << "Current Speed : " << currentSpeed << " km/s " << endl;
  }

  void Accel()
  {
    if (fuelGauge <= 0)
    {
      return;
    }
    else
    {
      fuelGauge -= FUEL_STEP;
    }

    if (currentSpeed + ACC_STEP >= MAX_SPD)
    {
      currentSpeed = MAX_SPD;
      return;
    }
    currentSpeed += ACC_STEP;
  }

  void Break()
  {
    if (currentSpeed < BRK_STEP)
    {
      currentSpeed = 0;
      return;
    }

    currentSpeed -= BRK_STEP;
  }
};

int main(void)
{
  Car run99 = {"run99", 100, 0};

  run99.Accel();
  run99.Accel();

  run99.ShowCarState();

  run99.Break();
  run99.ShowCarState();

  Car sped77 = {"sped99", 100, 0};

  sped77.Accel();
  sped77.Break();
  sped77.ShowCarState();

  return 0;
}
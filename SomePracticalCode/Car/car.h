#pragma once

class Car
{
private:
  int gasolineGauge;

public:
  Car(int gauge);
  int GetGasGauge();
};

class HybridCar : public Car
{
private:
  int electricGauge;

public:
  HybridCar(int gas, int elec);
  int GetElecGauge();
};

class HybridWaterCar : public HybridCar
{
private:
  int waterGauge;

public:
  HybridWaterCar(int gas, int elec, int water);
  void ShowCurrentGauge();
};
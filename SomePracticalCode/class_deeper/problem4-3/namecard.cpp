#include "namecard.h"

#include <iostream>

void POSITION_CONST::ShowPositionInfo(int position)
{
  switch (position)
  {
  case CLERK:
    std::cout << "CLERK" << std::endl;
  case SENIOR:
    std::cout << "SENIOR" << std::endl;
  case ASSIST:
    std::cout << "ASSIST" << std::endl;
  case MANAGER:
    std::cout << "MANAGER" << std::endl;
  }
}

NameCard::NameCard(
    const std::string name,
    const std::string company,
    const std::string phoneNumber,
    const int position)
    : name(name), company(company), phoneNumber(phoneNumber)
{
}

NameCard::~NameCard()
{
  std::cout << "Destructor!" << std::endl;
}

void NameCard::ShowNameCardInfo()
{
  std::cout << "Name : " << name << std::endl;
  std::cout << "Company : " << company << std::endl;
  std::cout << "Phone Number : " << phoneNumber << std::endl;
  std::cout << "Position : ";
  POSITION_CONST::ShowPositionInfo(position);
}
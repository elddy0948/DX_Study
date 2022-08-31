#pragma once
#include <string>

namespace POSITION_CONST
{
  enum
  {
    MANAGER,
    ASSIST,
    SENIOR,
    CLERK
  };

  void ShowPositionInfo(int position);
}

class NameCard
{
public:
  NameCard(
      const std::string name,
      const std::string company,
      const std::string phoneNumber,
      const int position);
  ~NameCard();
  void ShowNameCardInfo();

private:
  std::string name;
  std::string company;
  std::string phoneNumber;
  int position;
};
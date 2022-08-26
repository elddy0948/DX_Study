#include "namecard.h"

int main(void)
{
  NameCard manClerk("Lee", "ABCEng", "010-1111-2222", POSITION_CONST::CLERK);
  NameCard manSENIOR("Hong", "OrangeEng", "010-3333-2222", POSITION_CONST::SENIOR);
  NameCard manAssist("Joon", "PearlEng", "010-1234-5322", POSITION_CONST::ASSIST);

  manClerk.ShowNameCardInfo();
  manSENIOR.ShowNameCardInfo();
  manAssist.ShowNameCardInfo();

  return 0;
}
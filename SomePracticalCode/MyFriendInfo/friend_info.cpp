#include "friend_info.h"

#include <iostream>
#include <cstring>

using namespace std;

MyFriendInfo::MyFriendInfo(const char *name, int age)
    : age(age)
{
  this->name = new char[strlen(name) + 1];
  strcpy(this->name, name);
}

void MyFriendInfo::ShowMyFriendInfo() const
{
  cout << "Name : " << name << endl;
  cout << "Age : " << age << endl;
}

MyFriendInfo::~MyFriendInfo()
{
  cout << "Called MyFriendInfo Destructor" << endl;
  delete[] name;
}

MyFriendDetailInfo::MyFriendDetailInfo(
    const char *name, int age,
    const char *address, const char *phone)
    : MyFriendInfo(name, age)
{
  this->address = new char[strlen(address) + 1];
  this->phone = new char[strlen(phone) + 1];
  strcpy(this->address, address);
  strcpy(this->phone, phone);
}

void MyFriendDetailInfo::ShowMyFriendDetailInfo()
{
  ShowMyFriendInfo();

  cout << "Address : " << address << endl;
  cout << "Phone : " << phone << endl;
}

MyFriendDetailInfo::~MyFriendDetailInfo()
{
  cout << "Called MyFriendDetailInfo Destructor" << endl;

  delete[] address;
  delete[] phone;
}

int main(void)
{
  MyFriendDetailInfo detailInfo("Joons", 27, "Jangyu", "010-0123-4567");

  detailInfo.ShowMyFriendDetailInfo();

  return 0;
}
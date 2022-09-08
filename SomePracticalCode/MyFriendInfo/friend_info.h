#pragma once

class MyFriendInfo
{
private:
  char *name;
  int age;

public:
  MyFriendInfo(const char *name, int age);
  void ShowMyFriendInfo() const;
  ~MyFriendInfo();
};

class MyFriendDetailInfo : public MyFriendInfo
{
private:
  char *address;
  char *phone;

public:
  MyFriendDetailInfo(
      const char *name, int age,
      const char *address, const char *phone);
  void ShowMyFriendDetailInfo();
  ~MyFriendDetailInfo();
};
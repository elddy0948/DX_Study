#include <iostream>
#include <cstring>

using namespace std;

class Gun
{
public:
  Gun(int bullet)
      : bullet(bullet)
  {
  }

  void Shot()
  {
    cout << "Bang!" << endl;
    bullet--;
  }

private:
  int bullet;
};

class Police
{
private:
  int handcuffs;
  Gun *pistol;

public:
  Police(int bullet, int cuffs)
      : handcuffs(cuffs)
  {
    if (bullet > 0)
    {
      pistol = new Gun(bullet);
    }
    else
    {
      pistol = nullptr;
    }
  }

  Police(const Police &ref)
      : handcuffs(ref.handcuffs)
  {
    if (ref.pistol != nullptr)
      pistol = new Gun(*(ref.pistol));
    else
      pistol = nullptr;
  }

  Police &operator=(const Police &ref)
  {
    if (pistol != nullptr)
      delete pistol;

    if (ref.pistol != nullptr)
      pistol = new Gun(*(ref.pistol));
    else
      pistol = nullptr;

    handcuffs = ref.handcuffs;
    return *this;
  }

  void PutHandcuff()
  {
    cout << "Snap!" << endl;
    handcuffs--;
  }

  void Shot()
  {
    if (pistol == nullptr)
    {
      cout << "Hut BBANG!" << endl;
    }
    else
    {
      pistol->Shot();
    }
  }

  ~Police()
  {
    if (pistol != nullptr)
    {
      delete pistol;
      cout << "~Police()" << endl;
    }
  }
};

int main(void)
{
  Police pMan1(5, 3);
  Police pMan2 = pMan1;
  pMan2.Shot();
  pMan2.PutHandcuff();

  Police pMan3(2, 4);
  pMan3 = pMan1;
  pMan3.Shot();
  pMan3.PutHandcuff();

  return 0;
}
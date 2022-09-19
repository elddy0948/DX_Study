#include <iostream>
#include <cstring>

class Person
{
public:
  Person(char *name)
  {
    this->name = new char[strlen(name) + 1];
    strcpy(this->name, name);
  }

  void ShowName()
  {
    std::cout << name << std::endl;
  }

  Person &operator=(const Person &ref)
  {
    // 이미 가리키고 있던 name을 해제시켜줌
    delete[] name;

    // Deep copy
    int len = strlen(ref.name) + 1;
    name = new char[len];
    strcpy(name, ref.name);
    return *this;
  }

  ~Person()
  {
    delete[] name;
    std::cout << "~Person()" << std::endl;
  }

private:
  char *name;
};

int main(void)
{

  Person personOne("Joons");
  Person personTwo("Holuck");

  personTwo = personOne;

  personOne.ShowName();
  personTwo.ShowName();

  return 0;
}
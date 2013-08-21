#include <iostream>

class object{
public:
  int integer;
};

void func1(object o){
  o.integer = 1;
}
void func2(object& o){
  o.integer = 2;
}

int main(){
  object o;
  o.integer = 0;
  std::cout << o.integer << std::endl;
  func1(o);
  std::cout << o.integer << std::endl;
  func2(o);
  std::cout << o.integer << std::endl;
}

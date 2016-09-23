#include "variant.h"
#include <string>
#include <iostream>

struct Dropper {
  Dropper() {
    std::cout << "default constructor\n";
  }
  ~Dropper() { std::cout << "destructor\n"; }
  Dropper(Dropper const& other) {
    std::cout << "copy constructor\n";
  }
  Dropper& operator=(Dropper const& other) {
    std::cout << "copy assignment\n";
    return *this;
  }
  Dropper(Dropper&& other) {
    std::cout << "move constructor\n";
  }
  Dropper& operator=(Dropper&& other) {
    std::cout << "move assignment\n";
    return *this;
  }
};

adt(Test,
  First, Dropper,
  Second, std::string,
  Third, void)
end_adt()

int main() {
  auto t = Test::First({});
  t = Test::Second("Hello, world");
  t.match(Test::matcher()
    .Second([](auto x){std::cout << x << "\n";})
  );
}

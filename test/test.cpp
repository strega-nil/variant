#include "variant.h"
#include <string>
#include <iostream>

struct dropper {
  dropper() {
    std::cout << "default constructor\n";
  }
  ~dropper() { std::cout << "destructor\n"; }
  dropper(dropper const& other) {
    std::cout << "copy constructor\n";
  }
  dropper& operator=(dropper const& other) {
    std::cout << "copy assignment\n";
    return *this;
  }
  dropper(dropper&& other) {
    std::cout << "move constructor\n";
  }
  dropper& operator=(dropper&& other) {
    std::cout << "move assignment\n";
    return *this;
  }
};

adt(test,
  first, dropper,
  second, std::string)

end_adt()

int main() {
  auto t = test::first({});
  t = test::second("Hello, world");
  t.match(test::matcher()
    .second([](auto x){std::cout << x << "\n";})
  );
}

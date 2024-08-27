#include "allocator_tests.h"

int main() {
  using namespace cbl_tests;

  // Allocator tests
  {
    allocatorTests();
    fbaTests();
  }

  return 0;
}

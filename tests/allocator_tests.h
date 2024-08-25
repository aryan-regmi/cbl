#ifndef CBL_ALLOCATOR_TESTS_H
#define CBL_ALLOCATOR_TESTS_H

#include "cbl/mem/c_allocator.h"
#include "cbl/primitives.h"
#include "cbl/slice.h"
#include <cassert>

namespace cbl_tests {
using namespace cbl;
using namespace cbl::mem;

inline static void allocatorTests() {
  CAllocator allocator = CAllocator{};

  // Raw allocation
  {
    Layout    layout = Layout::init<int>();
    Slice<u8> mem    = allocator.allocate(layout);
    int*      val    = mem.as<int>();
    assert(val != nullptr);
    *val = 2;
    assert(*val == 2);
    allocator.deallocate(reinterpret_cast<u8*>(val), layout);
  }

  // Typesafe allocation
  {
    int* val = allocator.create<int>();
    assert(val != nullptr);
    *val = 2;
    assert(*val == 2);
    allocator.destroy(val);
  }

  // Array allocation
  {
    Slice<int> arr = allocator.createArray<int>(3);
    assert(!arr.isEmpty());
    arr[0] = 1;
    arr[1] = 2;
    arr[2] = 3;
    for (int i = 0; i < 3; i++) {
      assert(arr[i] == i + 1);
    }
    allocator.destroyArray(arr);
  }
}

} // namespace cbl_tests

#endif // !CBL_ALLOCATOR_TESTS_H

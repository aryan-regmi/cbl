#ifndef CBL_ALLOCATOR_TESTS_H
#define CBL_ALLOCATOR_TESTS_H

#include "cbl/mem/c_allocator.h"
#include "cbl/mem/fba.h"
#include "cbl/primitives.h"
#include "cbl/slice.h"
#include <cassert>
#include <cstdint>
#include <cstdio>

namespace cbl_tests {
using namespace cbl;
using namespace cbl::mem;

inline bool is_aligned(const void* ptr, uintptr_t alignment) noexcept {
  auto iptr = reinterpret_cast<uintptr_t>(ptr);
  return (iptr % alignment) == 0;
}

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

inline static void fbaTests() {

  const usize          BUFSIZE = 16;
  u8                   raw_buf[BUFSIZE];
  FixedBufferAllocator fba{Slice<u8>{raw_buf, BUFSIZE}};

  {
    Layout    layout{sizeof(int), 8};
    Slice<u8> mem = fba.allocate(layout);
    assert(is_aligned(mem.ptr(), 8));
    int* val = mem.as<int>();
    assert(val != nullptr);
    *val = 2;
    assert(*val == 2);
    fba.reset();
  }

  {
    int* mem = fba.create<int>();
    assert(mem != nullptr);
    *mem = 2;
    assert(*mem == 2);
  }
}

} // namespace cbl_tests

#endif // !CBL_ALLOCATOR_TESTS_H

#include "cbl/mem/allocator.h"

#include "cbl/assert.h"     // CBL_ASSERT
#include "cbl/primitives.h" // u8
#include "cbl/slice.h"      // Slice
#include <cstring>          // memset

namespace cbl::mem {

auto Allocator::allocateZeroed(Layout layout) noexcept -> Slice<u8> {
  Slice<u8> mem = this->allocate(layout);
  if (mem.isEmpty()) {
    return Slice<u8>{};
  }
  void* zeroed = std::memset(mem.ptr(), 0, layout.size());
  CBL_ASSERT(zeroed != nullptr, "`memset` returned null");
  return Slice<u8>{mem.ptr(), layout.size()};
}

auto Allocator::grow(u8* ptr, Layout old_layout,
                     Layout new_layout) noexcept -> Slice<u8> {
  CBL_ASSERT(ptr != nullptr, "`ptr` must not be null");
  CBL_ASSERT(new_layout.size() >= old_layout.size(),
             "`new_layout.size()` must be greater than or equal to "
             "`old_layout.size()`");

  // Allocate new memory
  Slice<u8> new_mem = this->allocate(new_layout);
  if (new_mem.isEmpty()) {
    return Slice<u8>{};
  }

  // Copy old data into new allocation
  void* copied = std::memcpy(new_mem.ptr(), ptr, old_layout.size());
  if (copied == nullptr) {
    return Slice<u8>{};
  }

  // Free old mem and return new mem
  this->deallocate(ptr, old_layout);
  return new_mem;
}

auto Allocator::growZeroed(u8* ptr, Layout old_layout,
                           Layout new_layout) noexcept -> Slice<u8> {
  CBL_ASSERT(ptr != nullptr, "`ptr` must not be null");
  CBL_ASSERT(new_layout.size() >= old_layout.size(),
             "`new_layout.size()` must be greater than or equal to "
             "`old_layout.size()`");

  // Allocate new memory
  Slice<u8> new_mem = this->allocateZeroed(new_layout);
  if (new_mem.isEmpty()) {
    return Slice<u8>{};
  }

  // Copy old data into new allocation
  void* copied = std::memcpy(new_mem.ptr(), ptr, old_layout.size());
  if (copied == nullptr) {
    return Slice<u8>{};
  }

  // Free old mem and return new mem
  this->deallocate(ptr, old_layout);
  return new_mem;
}

auto Allocator::shrink(u8* ptr, Layout old_layout,
                       Layout new_layout) noexcept -> Slice<u8> {
  CBL_ASSERT(ptr != nullptr, "`ptr` must not be null");
  CBL_ASSERT(
      new_layout.size() <= old_layout.size(),
      "`new_layout.size()` must be less than or equal to `old_layout.size()`");

  // Allocate new memory
  Slice<u8> new_mem = this->allocateZeroed(new_layout);
  if (new_mem.isEmpty()) {
    return Slice<u8>{};
  }

  // Copy old data into new allocation
  void* copied = std::memcpy(new_mem.ptr(), ptr, old_layout.size());
  if (copied == nullptr) {
    return Slice<u8>{};
  }

  // Free old mem and return new mem
  this->deallocate(ptr, old_layout);
  return new_mem;
}

} // namespace cbl::mem

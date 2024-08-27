#include "cbl/mem/fba.h"

#include "cbl/primitives.h" // u8
#include "cbl/slice.h"      // Slice
#include <cassert>          // assert
#include <cstdint>          // uintptr_t

namespace cbl::mem {

// TODO: CHECK ALIGNMENT
//  - Make sure the returned pointer is correclty aligned
// inline bool
// is_aligned(const void * ptr, std::uintptr_t alignment) noexcept {
//     auto iptr = reinterpret_cast<std::uintptr_t>(ptr);
//     return !(iptr % alignment);
//  }
auto FixedBufferAllocator::allocate(Layout layout) noexcept -> Slice<u8> {
  assert(layout.size() + layout.alignment() < this->_buf.len() - this->_pos);
  u8* mem = this->_buf.ptr() + this->_pos + layout.size() + layout.alignment();

  // Align allocation
  uintptr_t addr         = reinterpret_cast<uintptr_t>(mem);
  uintptr_t aligned_addr = addr & ~(layout.alignment() - 1);
  uintptr_t final_addr   = aligned_addr + layout.alignment();
  u8*       aligned_ptr  = reinterpret_cast<u8*>(final_addr);

  return Slice<u8>{aligned_ptr, layout.size()};
}

auto FixedBufferAllocator::deallocate(u8* ptr, Layout layout) noexcept -> void {
  assert(ownsPtr(ptr));
  this->_pos -= layout.size();
}

auto FixedBufferAllocator::reset() noexcept -> void { this->_pos = 0; }

auto FixedBufferAllocator::ownsPtr(const u8* ptr) const noexcept -> bool {
  uintptr_t ptr_addr = reinterpret_cast<uintptr_t>(ptr);
  uintptr_t buf_addr = reinterpret_cast<uintptr_t>(this->_buf.ptr());
  return (ptr_addr >= buf_addr) && (ptr_addr < buf_addr + this->_buf.len());
}

auto FixedBufferAllocator::ownsSlice(Slice<u8> slice) const noexcept -> bool {
  uintptr_t slice_addr = reinterpret_cast<uintptr_t>(slice.ptr());
  uintptr_t buf_addr   = reinterpret_cast<uintptr_t>(this->_buf.ptr());
  return ((slice_addr + slice.len() >= buf_addr) &&
          (slice_addr + slice.len() <= buf_addr + this->_buf.len()));
}

} // namespace cbl::mem

#include "cbl/mem/fba.h"

#include "cbl/assert.h"     // CBL_ASSERT
#include "cbl/primitives.h" // u8
#include "cbl/slice.h"      // Slice
#include <cstdint>          // uintptr_t
#include <stdckdint.h>      // ckd_sub, ckd_add

namespace cbl::mem {

FixedBufferAllocator::FixedBufferAllocator(Slice<u8> buf) noexcept
    : _pos{0}, _buf{buf} {}

auto FixedBufferAllocator::allocate(Layout layout) noexcept -> Slice<u8> {
  usize alloc_size;
  {
    bool invalid = ckd_add(&alloc_size, layout.size(),
                           static_cast<usize>(layout.alignment()));
    CBL_ASSERT(invalid == false, "Addition overflowed");

    usize remaining_mem;
    invalid = ckd_sub(&remaining_mem, this->_buf.len(), this->_pos);
    CBL_ASSERT(invalid == false, "Subtraction overflowed");

    CBL_ASSERT(
        alloc_size <= remaining_mem,
        "Not enough memory in the `FixedBufferAllocator` for the allocation");
  }
  u8*       mem          = this->_buf.ptr() + this->_pos + alloc_size;

  // Align allocation
  uintptr_t addr         = reinterpret_cast<uintptr_t>(mem);
  uintptr_t aligned_addr = addr & ~(layout.alignment() - 1);
  uintptr_t final_addr;
  {
    bool invalid = ckd_add(&final_addr, aligned_addr,
                           static_cast<usize>(layout.alignment()));
    CBL_ASSERT(invalid == false, "Addition overflowed");
  }
  u8*   aligned_ptr = reinterpret_cast<u8*>(final_addr);

  // Increment allocation position
  usize final_pos;
  {
    bool invalid = ckd_add(&final_pos, this->_pos, alloc_size);
    CBL_ASSERT(invalid == false, "Addition overflowed");
  }
  this->_pos = final_pos;
  return Slice<u8>{aligned_ptr, layout.size()};
}

auto FixedBufferAllocator::deallocate(u8* ptr, Layout layout) noexcept -> void {
  CBL_ASSERT(ownsPtr(ptr), "`ptr` was not allocated by this allocator");
  usize alloc_size;
  {
    bool invalid = ckd_add(&alloc_size, layout.size(),
                           static_cast<usize>(layout.alignment()));
    CBL_ASSERT(invalid == false, "Addition overflowed");
  }
  usize final_pos;
  {
    bool invalid = ckd_add(&final_pos, this->_pos, alloc_size);
    CBL_ASSERT(invalid == false, "Subtraction overflowed");
  }
  this->_pos = final_pos;
}

auto FixedBufferAllocator::reset() noexcept -> void { this->_pos = 0; }

auto FixedBufferAllocator::ownsPtr(const u8* ptr) const noexcept -> bool {
  uintptr_t ptr_addr = reinterpret_cast<uintptr_t>(ptr);
  uintptr_t buf_addr = reinterpret_cast<uintptr_t>(this->_buf.ptr());
  uintptr_t buf_end_addr;
  {
    bool invalid = ckd_add(&buf_end_addr, buf_addr, this->_buf.len());
    CBL_ASSERT(invalid == false, "Addition overflowed");
  }
  return (ptr_addr >= buf_addr) && (ptr_addr <= buf_end_addr);
}

auto FixedBufferAllocator::ownsSlice(Slice<u8> slice) const noexcept -> bool {
  uintptr_t slice_addr = reinterpret_cast<uintptr_t>(slice.ptr());
  uintptr_t buf_addr   = reinterpret_cast<uintptr_t>(this->_buf.ptr());
  uintptr_t slice_end_addr;
  {
    bool invalid = ckd_add(&slice_end_addr, slice_addr, slice.len());
    CBL_ASSERT(invalid == false, "Addition overflowed");
  }
  uintptr_t buf_end_addr;
  {
    bool invalid = ckd_add(&buf_end_addr, buf_addr, this->_buf.len());
    CBL_ASSERT(invalid == false, "Addition overflowed");
  }
  return ((slice_end_addr >= buf_addr) && (slice_end_addr <= buf_end_addr));
}

} // namespace cbl::mem

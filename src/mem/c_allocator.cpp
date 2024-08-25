#include "cbl/mem/c_allocator.h"
#include "cbl/primitives.h"

#include <cassert> // assert
#include <cstdint> // uintptr_t
#include <cstdlib> // malloc, free

namespace cbl::mem {

Slice<u8> CAllocator::allocate(Layout layout) noexcept {
  void* mem = std::malloc(layout.size() + layout.alignment());
  if (mem == nullptr) {
    return Slice<u8>{};
  }

  // Align allocation
  uintptr_t addr         = reinterpret_cast<uintptr_t>(mem);
  uintptr_t aligned_addr = addr & ~(layout.alignment() - 1);
  uintptr_t final_addr   = aligned_addr + layout.alignment();
  u8*       aligned_ptr  = reinterpret_cast<u8*>(final_addr);

  // Set offset
  u16       offset       = final_addr - addr;
  u16* offset_ptr = reinterpret_cast<u16*>(aligned_ptr - layout.alignment());
  *offset_ptr     = offset;

  return Slice<u8>{aligned_ptr, layout.size()};
}

void CAllocator::deallocate(u8* ptr, Layout layout) noexcept {
  assert(ptr != nullptr);
  u16* offset  = reinterpret_cast<u16*>(ptr - layout.alignment());
  u8*  alloced = ptr - *offset;
  std::free(alloced);
}

} // namespace cbl::mem

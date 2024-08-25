#ifndef CBL_MEM_C_ALLOCATOR_H
#define CBL_MEM_C_ALLOCATOR_H

#include "cbl/mem/allocator.h" // Allocator
#include "cbl/primitives.h"    // u8
#include "cbl/slice.h"         // Slice

namespace cbl::mem {

/// An allocator backed by the `malloc` and `free` functions.
struct CAllocator : public Allocator {
  explicit CAllocator() noexcept                    = default;
  CAllocator(CAllocator&&) noexcept                 = default;
  CAllocator(const CAllocator&) noexcept            = default;
  CAllocator& operator=(CAllocator&&) noexcept      = default;
  CAllocator& operator=(const CAllocator&) noexcept = default;
  ~CAllocator() noexcept                            = default;

public:
  /// Allocates memory using `malloc`.
  auto allocate(Layout layout) noexcept -> Slice<u8> override;

  /// Deallocates memory using `free`.
  auto deallocate(u8* ptr, Layout layout) noexcept -> void override;
};

} // namespace cbl::mem

#endif // !CBL_MEM_C_ALLOCATOR_H

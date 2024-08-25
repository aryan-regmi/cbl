#ifndef CBL_MEM_C_ALLOCATOR_H
#define CBL_MEM_C_ALLOCATOR_H

#include "cbl/mem/allocator.h" // Allocator

namespace cbl::mem {

/// An allocator backed by the `malloc` and `free` functions.
struct CAllocator : public Allocator {
  explicit CAllocator() noexcept                    = default;
  CAllocator(CAllocator&&) noexcept                 = default;
  CAllocator(const CAllocator&) noexcept            = default;
  CAllocator& operator=(CAllocator&&) noexcept      = default;
  CAllocator& operator=(const CAllocator&) noexcept = default;
  virtual ~CAllocator() noexcept                    = default;

public:
  /// Allocates memory using `malloc`.
  Slice<u8> allocate(Layout layout) noexcept override;

  /// Deallocates memory using `free`.
  void      deallocate(u8* ptr, Layout layout) noexcept override;
};

} // namespace cbl::mem

#endif // !CBL_MEM_C_ALLOCATOR_H

#ifndef CBL_MEM_ALLOCATOR_H
#define CBL_MEM_ALLOCATOR_H

#include "cbl/mem/layout.h" // Layout
#include "cbl/primitives.h" // u8
#include "cbl/slice.h"      // Slice

namespace cbl::mem {

struct Allocator {
  explicit Allocator() noexcept                   = default;
  Allocator(Allocator&&) noexcept                 = default;
  Allocator(const Allocator&) noexcept            = delete;
  Allocator& operator=(Allocator&&) noexcept      = default;
  Allocator& operator=(const Allocator&) noexcept = delete;
  virtual ~Allocator() noexcept                   = default;

public:
  virtual auto allocate(Layout layout) -> Slice<u8>       = 0;

  virtual auto deallocate(u8* ptr, Layout layout) -> void = 0;

  auto         allocateZeroed(Layout layout) -> Slice<u8>;

  auto         grow(u8* ptr, Layout old_layout, Layout new_layout) -> Slice<u8>;

  auto growZeroed(u8* ptr, Layout old_layout, Layout new_layout) -> Slice<u8>;

  auto shrink(u8* ptr, Layout old_layout, Layout new_layout) -> Slice<u8>;

  template <class T> auto create() -> T*;
};

} // namespace cbl::mem

#endif // !CBL_MEM_ALLOCATOR_H

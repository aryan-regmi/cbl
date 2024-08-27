#ifndef CBL_MEM_FBA_H
#define CBL_MEM_FBA_H

#include "cbl/mem/allocator.h" // Allocator
#include "cbl/primitives.h"
#include "cbl/slice.h"

namespace cbl::mem {

struct FixedBufferAllocator : public Allocator {
  explicit FixedBufferAllocator() noexcept                         = delete;
  FixedBufferAllocator(FixedBufferAllocator&&) noexcept            = default;
  FixedBufferAllocator(const FixedBufferAllocator&) noexcept       = delete;
  FixedBufferAllocator& operator=(FixedBufferAllocator&&) noexcept = default;
  FixedBufferAllocator&
  operator=(const FixedBufferAllocator&) noexcept = delete;
  ~FixedBufferAllocator() noexcept                = default;

public:
  /// Initialize `FixedBufferAllocator` from a buffer.
  explicit FixedBufferAllocator(Slice<u8> buf) noexcept;

  /// Allocates memory in the buffer.
  auto allocate(Layout layout) noexcept -> Slice<u8> override;

  /// Allocates memory from the buffer.
  auto deallocate(u8* ptr, Layout layout) noexcept -> void override;

  /// Resets the allocator.
  auto reset() noexcept -> void;

  /// Returns `true` if `ptr` was allocated by `this`.
  auto ownsPtr(const u8* ptr) const noexcept -> bool;

  /// Returns `true` if `slice` was allocated by `this`.
  auto ownsSlice(Slice<u8> slice) const noexcept -> bool;

private:
  usize     _pos;
  Slice<u8> _buf;
};

} // namespace cbl::mem

#endif // !CBL_MEM_FBA_H

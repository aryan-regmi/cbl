#ifndef CBL_MEM_FBA_H
#define CBL_MEM_FBA_H

#include "cbl/mem/allocator.h" // Allocator
#include "cbl/primitives.h"
#include "cbl/slice.h"

namespace cbl::mem {

struct Fba : public Allocator {
  explicit Fba() noexcept             = delete;
  Fba(Fba&&) noexcept                 = default;
  Fba(const Fba&) noexcept            = delete;
  Fba& operator=(Fba&&) noexcept      = default;
  Fba& operator=(const Fba&) noexcept = delete;
  ~Fba() noexcept                     = default;

public:
  explicit Fba(Slice<u8> buf) noexcept;

  /// Allocates memory in the buffer.
  auto allocate(Layout layout) noexcept -> Slice<u8> override;

  /// Allocates memory from the buffer.
  auto deallocate(u8* ptr, Layout layout) noexcept -> void override;

  /// Returns `true` if `ptr` was allocated by `this`.
  auto ownsPtr(u8* ptr) const noexcept -> bool;

  /// Returns `true` if `slice` was allocated by `this`.
  auto ownsSlice(Slice<u8> slice) const noexcept -> bool;

private:
  usize     _pos;
  Slice<u8> _buf;
};

} // namespace cbl::mem

#endif // !CBL_MEM_FBA_H

#ifndef CBL_MEM_LAYOUT_H
#define CBL_MEM_LAYOUT_H

#include "cbl/assert.h"     // CBL_ASSERT
#include "cbl/primitives.h" // usize, u16
#include <stdckdint.h>      // ckd_mul

namespace cbl::mem {

/// Describes a particular layout of memory.
///
/// # Note
///
/// * This requires an alignment that is a power of 2.
/// * The alignment is a `u16`, so the max allowed alignment is `65536`.
struct Layout {
  explicit Layout() noexcept                = default;
  Layout(Layout&&) noexcept                 = default;
  Layout(const Layout&) noexcept            = default;
  Layout& operator=(Layout&&) noexcept      = default;
  Layout& operator=(const Layout&) noexcept = default;
  ~Layout() noexcept                        = default;

public:
  /// Creates a new memory layout with the given size and alignment.
  explicit Layout(usize size, u16 alignment) noexcept;

  /// Creates a memory layout suitable for holding a value of type `T`.
  template <class T> static auto init() noexcept -> Layout {
    CBL_ASSERT(isPowerOf2(alignof(T)),
               "Type `T` must have an alignment that is a power of 2");
    return Layout{sizeof(T), alignof(T)};
  }

  /// Creates a memory layout for an array with `len` elements of type `T`.
  template <class T> static auto array(usize len) noexcept -> Layout {
    CBL_ASSERT(isPowerOf2(alignof(T)),
               "Type `T` must have an alignment that is a power of 2");
    usize final_size;
    bool  invalid = ckd_mul(&final_size, sizeof(T), len);
    CBL_ASSERT(invalid == false, "Multiplication overflowed");
    return Layout{final_size, alignof(T)};
  }

  /// Creates a memory layout sutible for holding `value`.
  ///
  /// # Note
  ///
  /// `value` must be a single item pointer and must not be `nullptr`.
  template <class T> static auto fromValue(T* value) noexcept -> Layout {
    CBL_ASSERT(value != nullptr, "`value` must not be null");
    CBL_ASSERT(isPowerOf2(alignof(T)),
               "Type `T` must have an alignment that is a power of 2");
    return Layout{sizeof(T), alignof(T)};
  }

  /// Creates a memory layout sutible for holding `arr`.
  ///
  /// # Note
  ///
  /// `arr` must be a multi-item pointer and must not be `nullptr`.
  template <class T>
  static auto fromArray(T* arr, usize len) noexcept -> Layout {
    CBL_ASSERT(arr != nullptr, "`arr` must not be null");
    CBL_ASSERT(isPowerOf2(alignof(T)),
               "Type `T` must have an alignment that is a power of 2");
    usize final_size;
    bool  invalid = ckd_mul(&final_size, sizeof(T), len);
    CBL_ASSERT(invalid == false, "Multiplication overflowed");
    return Layout{final_size, alignof(T)};
  }

  /// Gets the size in bytes.
  auto size() const noexcept -> usize;

  /// Gets the alignment in bytes.
  auto alignment() const noexcept -> u16;

private:
  usize       _size;
  u16         _alignment;

  /// Checks if an alignment is a power of 2.
  static auto isPowerOf2(u16 alignment) -> bool;
};

} // namespace cbl::mem

#endif // !CBL_MEM_LAYOUT_H

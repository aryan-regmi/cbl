#ifndef CBL_DYNAMIC_ARRAY_H
#define CBL_DYNAMIC_ARRAY_H

#include "cbl/mem/allocator.h"
#include "cbl/primitives.h" // usize

namespace cbl {

template <class T> struct UnmanagedDynamicArray {
  /// Creates an empty array.
  explicit UnmanagedDynamicArray() noexcept                          = default;
  UnmanagedDynamicArray(UnmanagedDynamicArray&&) noexcept            = default;
  UnmanagedDynamicArray(const UnmanagedDynamicArray&) noexcept       = delete;
  UnmanagedDynamicArray& operator=(UnmanagedDynamicArray&&) noexcept = default;
  UnmanagedDynamicArray&
  operator=(const UnmanagedDynamicArray&) noexcept = delete;
  ~UnmanagedDynamicArray() noexcept                = default;

public:
  /// Creates an array with memory reserved for `capacity` elements.
  explicit UnmanagedDynamicArray(mem::Allocator allocator,
                                 usize          capacity) noexcept;

  /// Returns the pointer to the array's elements.
  auto elems() const noexcept -> T*;

  /// Returns the number of elements in the array.
  auto len() const noexcept -> usize;

  /// Returns the number of elements that the array has reserved memory for.
  auto cap() const noexcept -> usize;

private:
  T*    _elems = nullptr;
  usize _len   = 0;
  usize _cap   = 0;
};

} // namespace cbl

#endif // !CBL_DYNAMIC_ARRAY_H

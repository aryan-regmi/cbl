#ifndef CBL_SLICE_H
#define CBL_SLICE_H

#include "cbl/assert.h"     // CBL_ASSERT
#include "cbl/primitives.h" // usize

namespace cbl {

/// A dynamically-size view into a contiguous sequence.
///
/// Slices are a view into a block of memory represented as a pointer and a
/// length.
template <class T> struct Slice {
  /// Creates an empty slice.
  explicit Slice() noexcept               = default;
  Slice(Slice&&) noexcept                 = default;
  Slice(const Slice&) noexcept            = default;
  Slice& operator=(Slice&&) noexcept      = default;
  Slice& operator=(const Slice&) noexcept = default;
  ~Slice() noexcept                       = default;

public:
  /// Creates a slice from a raw pointer and length.
  explicit Slice(T* ptr, usize len) noexcept : _ptr{ptr}, _len{len} {}

  /// Returns `true` if the slice is empty.
  auto isEmpty() const noexcept {
    return (this->_ptr == nullptr) || (this->_len == 0);
  }

  /// Returns the pointer to the underlying contiguous sequence.
  auto ptr() const noexcept -> T* { return this->_ptr; }

  /// Returns the number of elements in the sequence.
  auto len() const noexcept -> usize { return this->_len; };

  /// Gets an immutable pointer to the value of the slice at `idx`.
  ///
  /// # Note
  ///
  /// The `idx` must be less than the slice's length.
  auto getPtr(usize idx) const noexcept -> const T* {
    CBL_ASSERT(idx < this->_len, "The index is outside the slice's bounds");
    return reinterpret_cast<T*>(reinterpret_cast<u8*>(this->_ptr) +
                                (idx * sizeof(T)));
  }

  /// Gets a mutable pointer to the value of the slice at `idx`.
  ///
  /// # Note
  ///
  /// The `idx` must be less than the slice's length.
  auto getPtrMut(usize idx) noexcept -> T* {
    CBL_ASSERT(idx < this->_len, "The index is outside the slice's bounds");
    return reinterpret_cast<T*>(reinterpret_cast<u8*>(this->_ptr) +
                                (idx * sizeof(T)));
  }

  /// Returns a const reference to the value at `idx`.
  auto operator[](usize idx) const noexcept -> const T& { return *getPtr(idx); }

  /// Returns a reference to the value at `idx`.
  auto operator[](usize idx) noexcept -> T& { return *getPtrMut(idx); }

  /// Returns the slice as a `U*`.
  template <class U> auto as() noexcept -> U* {
    return reinterpret_cast<U*>(this->_ptr);
  }

private:
  T*    _ptr = 0;
  usize _len = 0;
};

} // namespace cbl

#endif // !CBL_SLICE_H

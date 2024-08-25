#ifndef CBL_SLICE_H
#define CBL_SLICE_H

#include "cbl/primitives.h" // usize

namespace cbl {

/// A dynamically-size view into a contiguous sequence.
///
/// Slices are a view into a block of memory represented as a pointer and a
/// length.
template <class T> struct Slice {
  explicit Slice() noexcept               = default;
  Slice(Slice&&) noexcept                 = default;
  Slice(const Slice&) noexcept            = default;
  Slice& operator=(Slice&&) noexcept      = default;
  Slice& operator=(const Slice&) noexcept = default;
  ~Slice() noexcept                       = default;

public:
  /// Returns the pointer to the underlying contiguous sequence.
  auto ptr() const noexcept -> usize { return this->_ptr; }

  /// Returns the number of elements in the sequence.
  auto len() const noexcept -> usize { return this->_len; };

private:
  T*    _ptr = 0;
  usize _len = 0;
};

} // namespace cbl

#endif // !CBL_SLICE_H

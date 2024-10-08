#ifndef CBL_DYNAMIC_ARRAY_H
#define CBL_DYNAMIC_ARRAY_H

#include "cbl/assert.h"
#include "cbl/mem/allocator.h"
#include "cbl/primitives.h" // usize
#include "cbl/slice.h"
#include <cstring>

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
  static auto
  initWithCapacity(mem::Allocator& allocator,
                   usize           capacity) noexcept -> UnmanagedDynamicArray {
    T* elems = allocator.createArray<T>(capacity);
    CBL_ASSERT(elems != nullptr, "Allocation failed");

    UnmanagedDynamicArray self;
    self._elems = elems;
    self._len   = 0;
    self._cap   = capacity;

    return self;
  }

  /// Creates an array from the given slice.
  ///
  /// This takes ownership of the slice.
  ///
  /// # Safety
  ///
  /// The call to `deinit` must use the same allocator that initialized the
  /// slice.
  static auto
  initFromOwnedSlice(Slice<T> slice) noexcept -> UnmanagedDynamicArray {
    UnmanagedDynamicArray self;
    self._elems = slice.ptr();
    self._len   = slice.len();
    self._cap   = slice.len();
    return self;
  }

  /// Frees all memory allocated by the array.
  ///
  /// # Note
  ///
  /// Calling just the destructor will result in a memory leak; `deinit` must be
  /// called to free allocated memory.
  auto deinit(mem::Allocator& allocator) noexcept -> void {
    allocator.deallocate(this->_elems);
    this->_elems = nullptr;
    this->_len   = 0;
    this->_cap   = 0;
  }

  /// Returns the array as a slice that the caller owns.
  ///
  /// # Note
  ///
  /// This will empty the array and clear its capacity.
  auto toOwnedSlice(mem::Allocator& allocator) noexcept -> Slice<T> {
    const usize len     = this->len;
    T*          new_mem = allocator.createArray<T>(len);
    void*       copied =
        std::memcpy(new_mem, this->_elems,
                    sizeof(T) * len); // TODO: Replace with loop instead?
    CBL_ASSERT(copied != nullptr, "`memcpy` failed");
    this->deinit(allocator);
    return Slice<T>{new_mem, len};
  }

  /// Returns the array's elements.
  ///
  /// # Note
  ///
  /// The array owns the elements of the returned slice.
  ///
  /// # Safety
  ///
  /// The returned slice will be invalid after any operations that change the
  /// size of the array (such as appends, removes, inserts, etc).
  auto elems() const noexcept -> Slice<T> {
    return Slice<T>{this->_elems, this->_len};
  }

  /// Returns the number of elements in the array.
  auto len() const noexcept -> usize { return this->_len; }

  /// Returns the number of elements that the array has reserved memory for.
  auto cap() const noexcept -> usize { return this->_cap; }

  /// Creates a copy of the array.
  auto
  clone(mem::Allocator& allocator) const noexcept -> UnmanagedDynamicArray {
    UnmanagedDynamicArray cloned =
        UnmanagedDynamicArray::initWithCapacity(allocator, this->_cap);
    cloned.appendSlice(allocator, this->elems());
    return cloned;
  }

  /// Inserts `value` at the specified index.
  ///
  /// Shifts all elements from `idx` to the right.
  ///
  /// If `idx` is equal to the length of the array, this is equivalent to
  /// `append`.
  ///
  /// This operation is O(N).
  ///
  /// # Safety
  ///
  /// Invalidates element pointers if additional memory is needed.
  auto insert(mem::Allocator& allocator, T value, usize idx) noexcept -> void {
    if (idx == this->_len) {
      this->append(allocator, value);
      return;
    }

    // Resize original buffer if necessary
    if (this->_len + 1 > this->_cap) {
      resize(allocator);
    }

    // Shift all elements from the idx to the right
    for (usize i = this->_len; i > idx; i--) {
      this->_elems[i] = this->_elems[i - 1];
    }

    // Insert value at `idx`
    this->_elems[idx]  = value;
    this->_len        += 1;
  }

  /// Insert `slice` at the specified index.
  ///
  /// Shifts all elements from `idx` to the right.
  ///
  /// If `idx` is equal to the length of the array, this is equivalent to
  /// `appendSlice`.
  ///
  /// This operation is O(N).
  ///
  /// # Safety
  ///
  /// * Invalidates pre-existing pointers to elements at and after `index`.
  /// * Invalidates all pre-existing element pointers if capacity must be
  ///   increased to accommodate the new elements.
  auto insertSlice(mem::Allocator& allocator, Slice<T> slice,
                   usize idx) noexcept -> void {
    if (idx == this->_len) {
      this->appendSlice(allocator, slice);
      return;
    }

    // Resize the original buffer if necessary
    while (this->_len + slice.len() > this->_cap) {
      resize(allocator);
    }

    // Shift all elements from the idx to the right, leaving enough space for
    // the slice elements
    for (usize i = this->_len + slice.len() - 1; i > idx; i--) {
      this->_elems[i] = this->_elems[i - slice.len()];
    }

    // Insert slice at `idx`
    for (usize i = idx; i < slice.len(); i++) {
      this->_elems[i] = slice[i - idx];
    }
    this->_len += slice.len();
  }

  /// Inserts `value` to the end of the array.
  ///
  /// Allocates more memory as necessary.
  ///
  /// # Safety
  ///
  /// Invalidates element pointers if additional memory is needed.
  auto append(mem::Allocator& allocator, T value) noexcept -> void {
    // Resize original buffer if necessary
    if (this->_len + 1 > this->_cap) {
      resize(allocator);
    }

    this->_elems[this->_len]  = value;
    this->_len               += 1;
  }

  /// Inserts `slice` to the end of the array.
  ///
  /// Allocates more memory as necessary.
  ///
  /// # Safety
  ///
  /// Invalidates element pointers if additional memory is needed.
  auto appendSlice(mem::Allocator& allocator, Slice<T> slice) noexcept -> void {
    // Resize original buffer if necessary
    while (this->_len + slice.len() > this->_cap) {
      resize(allocator);
    }

    for (usize i = 0; i < slice.len(); i++) {
      this->_elems[this->_len + i] = slice[i];
    }
    this->_len += slice.len();
  }

  /// Removes and returns the element at `idx`.
  ///
  /// Shifts all elements from `idx` to the left.
  ///
  /// This operation is O(N).
  ///
  /// # Note
  ///
  /// This preserves the order of the array elements.
  ///
  /// # Safety
  ///
  /// Invalidates pointers to the last element.
  auto remove(usize idx) -> T;

  /// Removes and returns the element at `idx`.
  ///
  /// Replaces the empty index with the last element of the array.
  ///
  /// This operation is O(1).
  ///
  /// # Safety
  ///
  /// Invalidates pointers to last element.
  auto swapRemove(usize idx) -> T;

private:
  T*    _elems = nullptr;
  usize _len   = 0;
  usize _cap   = 0;

  /// Resizes the array by doubling its capacity.
  ///
  /// # Safety
  ///
  /// This will invalidate all pointers to elements.
  auto  resize(mem::Allocator& allocator) -> void {
    usize cap = this->_cap;
    if (this->_cap == 0) {
      cap = 1;
    } else {
      cap *= 2;
    }

    T* resized = allocator.createArray<T>(cap);
    CBL_ASSERT(resized != nullptr, "Resize failed (out of memory)");

    // Copy and delete old data
    const usize len    = this->_len;
    void*       copied = std::memcpy(resized, this->_elems, sizeof(T) * len);
    CBL_ASSERT(copied != nullptr, "`memcpy` failed");
    this->deinit(allocator);

    this->_elems = resized;
    this->_len   = len;
    this->_cap   = cap;
  }
};

} // namespace cbl

#endif // !CBL_DYNAMIC_ARRAY_H

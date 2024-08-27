#ifndef CBL_MEM_ALLOCATOR_H
#define CBL_MEM_ALLOCATOR_H

#include "cbl/mem/layout.h" // Layout
#include "cbl/primitives.h" // u8, usize
#include "cbl/slice.h"      // Slice

namespace cbl::mem {

/// An interface for allocators.
///
///
/// # Note
///
/// Allocators must define `allocate` and `deallocate` functions.
struct Allocator {
  explicit Allocator() noexcept                   = default;
  Allocator(Allocator&&) noexcept                 = default;
  Allocator(const Allocator&) noexcept            = default;
  Allocator& operator=(Allocator&&) noexcept      = default;
  Allocator& operator=(const Allocator&) noexcept = default;
  virtual ~Allocator() noexcept                   = default;

public:
  /// Allocate memory as described by the given layout.
  ///
  /// Returns a slice to the newly allocated memory.
  ///
  /// The allocated block of memory may or may not be initialized.
  ///
  /// # Errors
  ///
  /// Returns an empty slice if either memory is exhausted or `layout` does not
  /// meet the allocator's size or alignment constraints.
  virtual auto allocate(Layout layout) noexcept -> Slice<u8>       = 0;

  /// Deallocates the memory referenced by `ptr`.
  ///
  /// # Safety
  ///
  /// * `ptr` must denote a block of memory *currently allocated* via this
  /// allocator
  /// * `layout` must *fit* that block of memory.
  virtual auto deallocate(u8* ptr, Layout layout) noexcept -> void = 0;

  /// Behaves like `allocate`, but also ensures that the returned
  /// memory is zero-initialized.
  ///
  /// # Errors
  ///
  /// Returns an empty slice if either memory is exhausted or `layout` does not
  /// meet the allocator's size or alignment constraints.
  auto         allocateZeroed(Layout layout) noexcept -> Slice<u8>;

  /// Attempts to extend the memory block.
  ///
  /// If this returns a non-empty slice, then ownership of the memory block
  /// referenced by `ptr` has been transferred to this allocator. Any access to
  /// the old `ptr` is Undefined Behavior, even if the allocation was grown
  /// in-place. The newly returned pointer is the only valid pointer for
  /// accessing this memory now.
  ///
  /// If this method returns an empty slice, then ownership of the memory block
  /// has not been transferred to this allocator, and the contents of the memory
  /// block are unaltered.
  ///
  /// # Safety
  ///
  /// * `ptr` must denote a block of memory *currently allocated* via this
  /// allocator.
  /// * `old_layout` must *fit* that block of memory (The `new_layout` argument
  /// need not fit it.).
  /// * `new_layout.size` must be greater than or equal to
  /// `old_layout.size`.
  ///
  ///
  /// # Errors
  ///
  /// Returns an empty slice if the new layout does not meet the allocator's
  /// size and alignment constraints of the allocator, or if growing otherwise
  /// fails.
  ///
  /// # Note
  ///
  /// Note that `new_layout.alignment` need not be the same as
  /// `old_layout.alignment`.
  auto         grow(u8* ptr, Layout old_layout,
                    Layout new_layout) noexcept -> Slice<u8>;

  /// Behaves like `grow`, but also ensures that the new contents
  /// are set to zero before being returned.
  ///
  /// # Safety
  ///
  /// * `ptr` must denote a block of memory *currently allocated* via this
  /// allocator.
  /// * `old_layout` must *fit* that block of memory (The `new_layout` argument
  /// need not fit it.).
  /// * `new_layout.size` must be greater than or equal to
  /// `old_layout.size`.
  ///
  ///
  /// # Errors
  ///
  /// Returns an empty slice if the new layout does not meet the allocator's
  /// size and alignment constraints of the allocator, or if growing otherwise
  /// fails.
  ///
  /// # Note
  ///
  /// Note that `new_layout.alignment` need not be the same as
  /// `old_layout.alignment`.
  auto         growZeroed(u8* ptr, Layout old_layout,
                          Layout new_layout) noexcept -> Slice<u8>;

  /// Attempts to shrink the memory block.
  ///
  /// If this returns a non-empty slice, then ownership of the memory block
  /// referenced by `ptr` has been transferred to this allocator. Any access to
  /// the old `ptr` is Undefined Behavior, even if the allocation was shrunk
  /// in-place. The newly returned pointer is the only valid pointer for
  /// accessing this memory now.
  ///
  /// If this method returns an empty slice, then ownership of the memory block
  /// has not been transferred to this allocator, and the contents of the memory
  /// block are unaltered.
  ///
  /// # Safety
  ///
  /// * `ptr` must denote a block of memory *currently allocated* via this
  /// allocator.
  /// * `old_layout` must *fit* that block of memory (The `new_layout` argument
  /// need not fit it.).
  /// * `new_layout.size` must be smaller than or equal to
  /// `old_layout.size`.
  ///
  /// # Errors
  ///
  /// Returns an empty slice if the new layout does not meet the allocator's
  /// size and alignment constraints of the allocator, or if shrinking otherwise
  /// fails.
  ///
  /// # Note
  ///
  /// Note that `new_layout.alignment` need not be the same as
  /// `old_layout.alignment`.
  auto         shrink(u8* ptr, Layout old_layout,
                      Layout new_layout) noexcept -> Slice<u8>;

  /// Allocates memory for a single object of type `T`.
  ///
  /// The memory must be freed by calling `destroy`.
  ///
  /// # Errors
  ///
  /// Returns `nullptr` if the allocation fails.
  template <class T> auto create() noexcept -> T* {
    Layout    layout = Layout::init<T>();
    Slice<u8> mem    = this->allocateZeroed(layout);
    return mem.as<T>();
  }

  /// Dellocates memory allocated by `Allocator::create`.
  ///
  /// # Safety
  ///
  /// Calls function `deallocate`, so it must abide by its safety
  /// considerations.
  template <class T> auto destroy(T* ptr) noexcept -> void {
    if (ptr != nullptr) {
      this->deallocate(reinterpret_cast<u8*>(ptr), Layout::init<T>());
    }
  }

  /// Allocates memory for an array with `len` elements of type `T`.
  ///
  /// The returned reference must be freed by calling
  /// `Allocator::destroyArray`.
  ///
  /// # Errors
  ///
  /// Returns an empty slice if the allocation fails.
  template <class T> auto createArray(usize len) noexcept -> Slice<T> {
    Layout    layout = Layout::array<T>(len);
    Slice<u8> mem    = this->allocateZeroed(layout);
    T*        ptr    = mem.as<T>();
    return Slice<T>{ptr, len};
  }

  /// Dellocates memory allocated by `createArray`.
  ///
  /// # Safety
  ///
  /// Calls function `deallocate`, so it must abide by its safety
  /// considerations.
  template <class T> auto destroyArray(Slice<T> slice) noexcept -> void {
    if (slice.isEmpty()) {
      return;
    }
    u8*    ptr    = reinterpret_cast<u8*>(slice.ptr());
    Layout layout = Layout::array<T>(slice.len());
    this->deallocate(ptr, layout);
  }
};

} // namespace cbl::mem

#endif // !CBL_MEM_ALLOCATOR_H

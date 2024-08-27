#include "cbl/mem/layout.h"

#include "cbl/assert.h"     // CBL_ASSERT
#include "cbl/primitives.h" // usize, u16

namespace cbl::mem {

Layout::Layout(usize size, u16 alignment) noexcept
    : _size{size}, _alignment{alignment} {
  CBL_ASSERT(isPowerOf2(alignment),
             "Type `T` must have an alignment that is a power of 2");
}

auto Layout::size() const noexcept -> usize { return this->_size; }

auto Layout::alignment() const noexcept -> u16 { return this->_alignment; }

auto Layout::isPowerOf2(u16 alignment) -> bool {
  return (alignment != 0) && ((alignment & (alignment - 1)) == 0);
}

} // namespace cbl::mem

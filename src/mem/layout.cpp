#include "cbl/mem/layout.h"

namespace cbl::mem {

Layout::Layout(usize size, u16 alignment) noexcept
    : _size{size}, _alignment{alignment} {}

auto Layout::size() const noexcept -> usize { return this->_size; }

auto Layout::alignment() const noexcept -> u16 { return this->_alignment; }

auto Layout::isPowerOf2(u16 alignment) -> bool {
  return (alignment != 0) && ((alignment & (alignment - 1)) == 0);
}

} // namespace cbl::mem

#include "cbl/io/writer.h"

#include "cbl/primitives.h" // usize
#include <cassert>          // assert
#include <cstdarg>          // va_list, va_start, va_end

namespace cbl::io {

auto Writer::format(const_cstr fmt, ...) noexcept -> void {
  assert(fmt != nullptr);
  std::va_list args;
  va_start(args, fmt);
  this->formatV(fmt, args);
  va_end(args);
}

auto Writer::writeAll(Slice<u8> buf) noexcept -> void {
  assert(!buf.isEmpty());
  usize idx = 0;
  while (idx != buf.len()) {
    idx += this->write(buf);
    buf  = Slice{buf.ptr() + idx, buf.len()};
  }
}

} // namespace cbl::io

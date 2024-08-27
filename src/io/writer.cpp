#include "cbl/io/writer.h"

#include "cbl/assert.h"     // CBL_ASSERT
#include "cbl/primitives.h" // usize
#include <cstdarg>          // va_list, va_start, va_end
#include <stdckdint.h>      // ckd_add

namespace cbl::io {

auto Writer::format(const_cstr fmt, ...) noexcept -> void {
  CBL_ASSERT(fmt != nullptr, "The format string must not be null");
  std::va_list args;
  va_start(args, fmt);
  this->formatV(fmt, args);
  va_end(args);
}

auto Writer::writeAll(Slice<u8> buf) noexcept -> void {
  if (!buf.isEmpty()) {
    usize idx = 0;
    while (idx != buf.len()) {
      usize written = this->write(buf);

      usize final_idx;
      bool  invalid = ckd_add(&final_idx, idx, written);
      CBL_ASSERT(invalid == false, "Addition overflowed");
      idx = final_idx;

      buf = Slice{buf.ptr() + final_idx, buf.len()};
    }
  }
}

} // namespace cbl::io

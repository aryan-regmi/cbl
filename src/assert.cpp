#include "assert.h"

#include "cbl/primitives.h" // const_cstr, usize
#include <cstdio>           // stderr, fprintf
#include <source_location>  // source_location

namespace cbl {

auto cbl_assert(bool expr, const_cstr msg,
                std::source_location loc = std::source_location::current())
    -> void {
  if (!expr) {
    std::fprintf(stderr, "[%s:%d:%d] %s\n", loc.file_name(), // NOLINT
                 loc.line(), loc.column(), msg);
  }
}

} // namespace cbl

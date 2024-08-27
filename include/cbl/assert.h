#ifndef CBL_ASSERT_H
#define CBL_ASSERT_H

#include "cbl/primitives.h" // const_cstr
#include <source_location>  // source_location

namespace cbl {

auto cbl_assert(bool expr, const_cstr msg,
                std::source_location loc = std::source_location::current())
    -> void;

// clang-format off
#ifdef CBL_ASSERT_ON
  #define CBL_ASSERT(expr, msg) ::cbl::cbl_assert(expr, msg)
#else
  #define CBL_ASSERT(expr, msg)
#endif // CBL_ASSERT_ON
// clang-format on

} // namespace cbl

#endif // !CBL_ASSERT_H

#ifndef CBL_PRIMITIVES_H
#define CBL_PRIMITIVES_H

#include <cstddef> // size_t

namespace cbl {

typedef _BitInt(8) u8;
typedef _BitInt(16) u16;
typedef _BitInt(32) u32;
typedef _BitInt(64) u64;
typedef std::size_t usize;

typedef signed _BitInt(8) i8;
typedef signed _BitInt(16) i16;
typedef signed _BitInt(32) i32;
typedef signed _BitInt(64) i64;
typedef signed long isize;

typedef float       f32;
typedef double      f64;

typedef char*       cstr;
typedef const char* const_cstr;

typedef void*       any;
typedef const void* const_any;

} // namespace cbl

#endif // !CBL_PRIMITIVES_H

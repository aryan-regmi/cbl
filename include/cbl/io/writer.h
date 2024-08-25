#ifndef CBL_IO_WRITER_H
#define CBL_IO_WRITER_H

#include "cbl/primitives.h" // u8, const_cstr
#include "cbl/slice.h"      // Slice
#include <cstdarg>          // va_list

namespace cbl::io {

struct Writer {
  explicit Writer() noexcept                = default;
  Writer(Writer&&) noexcept                 = default;
  Writer(const Writer&) noexcept            = default;
  Writer& operator=(Writer&&) noexcept      = default;
  Writer& operator=(const Writer&) noexcept = default;
  virtual ~Writer() noexcept                = default;

public:
  /// Writes the buffer into the writer, returning the number of bytes
  /// written.
  [[nodiscard]] virtual auto write(Slice<u8> buf) noexcept -> usize        = 0;

  /// Writes a formatted string into the writer.
  virtual auto formatV(const_cstr fmt, std::va_list args) noexcept -> void = 0;

  /// Writes a formatted string into the writer.
  auto         format(const_cstr fmt, ...) noexcept -> void;

  /// Attempts to write the entire buffer into the writer.
  auto         writeAll(Slice<u8> buf) noexcept -> void;
};

} // namespace cbl::io

#endif // !CBL_IO_WRITER_H

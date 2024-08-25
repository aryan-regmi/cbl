#ifndef CBL_IO_FILE_H
#define CBL_IO_FILE_H

#include "cbl/io/writer.h"  // Writer
#include "cbl/primitives.h" // const_cstr
#include <cstdarg>          // va_list
#include <cstdio>           // FILE

namespace cbl::io {

struct File : public Writer {
  enum class Mode {
    Read,
    Write,
    Append,
    ReadExtended,
    WriteExtended,
    AppendExtended,
  };

  explicit File() noexcept              = delete;
  File(File&&) noexcept                 = default;
  File(const File&) noexcept            = delete;
  File& operator=(File&&) noexcept      = default;
  File& operator=(const File&) noexcept = delete;

public:
  /// Creates a `File` from a raw `FILE*`.
  explicit File(std::FILE* file) noexcept;

  /// Create/open the file with `filename` in the specified mode.
  explicit File(const_cstr filename, Mode mode) noexcept;

  /// Cleanup resources used by `File`.
  ~File() noexcept;

  /// Writes the buffer into the file, returning the number of bytes written.
  [[nodiscard]] auto write(Slice<u8> buf) noexcept -> usize override;

  /// Writes a formatted string into the file.
  auto formatV(const_cstr fmt, std::va_list args) noexcept -> void override;

  /// Returns the raw `FILE*`.
  auto file() const noexcept -> std::FILE*;

  /// Clones the file.
  auto clone() const noexcept -> File;

private:
  std::FILE*  _file = nullptr;
  Mode        _mode = Mode::Read;

  /// Get the file mode as a C-string.
  static auto getFileMode(Mode mode) -> const_cstr;
};

/// Safe representation of `stdout`.
struct Stdout : public Writer {
  explicit Stdout() noexcept                = default;
  Stdout(Stdout&&) noexcept                 = default;
  Stdout(const Stdout&) noexcept            = default;
  Stdout& operator=(Stdout&&) noexcept      = default;
  Stdout& operator=(const Stdout&) noexcept = default;

public:
  /// Writes the buffer to `stdout`, returning the number of bytes written.
  [[nodiscard]] auto write(Slice<u8> buf) noexcept -> usize override;

  /// Writes a formatted string into `stdout`.
  auto formatV(const_cstr fmt, std::va_list args) noexcept -> void override;
};

/// Safe representation of `stderr`.
struct Stderr : public Writer {
  explicit Stderr() noexcept                = default;
  Stderr(Stderr&&) noexcept                 = default;
  Stderr(const Stderr&) noexcept            = default;
  Stderr& operator=(Stderr&&) noexcept      = default;
  Stderr& operator=(const Stderr&) noexcept = default;

public:
  /// Writes the buffer to `stderr`, returning the number of bytes written.
  [[nodiscard]] auto write(Slice<u8> buf) noexcept -> usize override;

  /// Writes a formatted string into `stderr`.
  auto formatV(const_cstr fmt, std::va_list args) noexcept -> void override;
};

// TODO: Add Stdin that implements Reader

} // namespace cbl::io

#endif // !CBL_IO_FILE_H

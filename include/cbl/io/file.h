#ifndef CBL_IO_FILE_H
#define CBL_IO_FILE_H

#include "cbl/io/writer.h"  // Writer
#include "cbl/primitives.h" // const_cstr
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

  ~File() noexcept;

private:
  std::FILE*  file = nullptr;
  Mode        mode = Mode::Read;

  /// Get the file mode as a C-string.
  static auto getFileMode(Mode mode) -> const_cstr;
};

} // namespace cbl::io

#endif // !CBL_IO_FILE_H

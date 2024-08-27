#include "cbl/io/file.h"

#include "cbl/assert.h"     // CBL_ASSERT
#include "cbl/primitives.h" // usize, const_cstr
#include <cstdio>           // FILE, stdout, stderr, stdin, fwrite, vfprintf
#include <unistd.h>         // dup

namespace cbl::io {

File::File(std::FILE* file) noexcept : _file{file} {}

File::File(const_cstr filename, Mode mode) noexcept {
  std::FILE* file = std::fopen(filename, getFileMode(mode));
  CBL_ASSERT(file != nullptr, "Failed to open the file");
  this->_file = file;
  this->_mode = mode;
}

File::~File() noexcept {
  if ((this->_file != stdout) && (this->_file != stderr) &&
      (this->_file != stdin)) {
    int closed = std::fclose(this->_file);
    CBL_ASSERT((closed == 0) || (closed == EOF), "Failed to close the file");
  }
}

[[nodiscard]] auto File::write(Slice<u8> buf) noexcept -> usize {
  if (!buf.isEmpty()) {
    return std::fwrite(buf.ptr(), sizeof(u8), buf.len(), this->_file);
  }
  return 0;
}

auto File::formatV(const_cstr fmt, std::va_list args) noexcept -> void {
  CBL_ASSERT(fmt != nullptr, "The format string must not be null");
  usize written = std::vfprintf(this->_file, fmt, args);
  CBL_ASSERT(written != 0, "Failed to write to file");
}

auto File::file() const noexcept -> std::FILE* { return this->_file; }

auto File::clone() const noexcept -> File {
  FILE* copied_file =
      fdopen(dup(fileno(this->_file)), getFileMode(this->_mode));
  return File{copied_file};
}

auto File::getFileMode(Mode mode) -> const_cstr {
  switch (mode) {
  case Mode::Write:
    return "w";
  case Mode::Append:
    return "a";
  case Mode::ReadExtended:
    return "r+";
  case Mode::WriteExtended:
    return "w+";
  case Mode::AppendExtended:
    return "a+";
  case Mode::Read:
  default:
    return "r";
  }
}

[[nodiscard]] auto Stdout::write(Slice<u8> buf) noexcept -> usize {
  if (!buf.isEmpty()) {
    return std::fwrite(buf.ptr(), sizeof(u8), buf.len(), stdout);
  }
  return 0;
}

auto Stdout::formatV(const_cstr fmt, std::va_list args) noexcept -> void {
  CBL_ASSERT(fmt != nullptr, "The format string must not be null");
  usize written = std::vfprintf(stdout, fmt, args);
  CBL_ASSERT(written != 0, "Failed to write to stdout");
}

[[nodiscard]] auto Stderr::write(Slice<u8> buf) noexcept -> usize {
  if (!buf.isEmpty()) {
    return std::fwrite(buf.ptr(), sizeof(u8), buf.len(), stderr);
  }
  return 0;
}

auto Stderr::formatV(const_cstr fmt, std::va_list args) noexcept -> void {
  CBL_ASSERT(fmt != nullptr, "The format string must not be null");
  usize written = std::vfprintf(stderr, fmt, args);
  CBL_ASSERT(written != 0, "Failed to write to stdout");
}

} // namespace cbl::io

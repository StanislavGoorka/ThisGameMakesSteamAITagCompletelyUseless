#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <utility>

namespace closed_circuit::windows
{
class FileHandle
{
  public:
    explicit FileHandle(HANDLE handle) noexcept : handle_(handle)
    {
    }
    FileHandle(const FileHandle &) = delete;
    FileHandle &operator=(const FileHandle &) = delete;
    ~FileHandle()
    {
        static_cast<void>(close());
    }

    [[nodiscard]] HANDLE get() const noexcept
    {
        return handle_;
    }
    [[nodiscard]] explicit operator bool() const noexcept
    {
        return handle_ != nullptr && handle_ != INVALID_HANDLE_VALUE;
    }
    [[nodiscard]] bool close() noexcept
    {
        if (!*this)
            return true;
        return CloseHandle(std::exchange(handle_, INVALID_HANDLE_VALUE)) != 0;
    }

  private:
    HANDLE handle_;
};
} // namespace closed_circuit::windows

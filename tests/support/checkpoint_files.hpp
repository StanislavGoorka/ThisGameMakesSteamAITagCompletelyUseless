#pragma once

#include "platform/windows/file_handle.hpp"
#include <filesystem>
#include <fstream>
#include <iterator>
#include <span>
#include <stdexcept>
#include <vector>

namespace closed_circuit::test
{
class CheckpointDirectory
{
  public:
    CheckpointDirectory()
    {
        path = std::filesystem::current_path() /
               (L"checkpoint_test_" + std::to_wstring(GetCurrentProcessId()));
        // Never delete a pre-existing path. CTest runs each test in its own
        // process, and each fixture owns only the directory it created.
        if (!std::filesystem::create_directory(path))
            throw std::runtime_error("Test directory already exists");
    }
    CheckpointDirectory(const CheckpointDirectory &) = delete;
    CheckpointDirectory &operator=(const CheckpointDirectory &) = delete;
    ~CheckpointDirectory()
    {
        std::error_code error;
        std::filesystem::remove_all(path, error);
    }
    std::filesystem::path path;
};

inline void write_bytes(const std::filesystem::path &path,
                        std::span<const std::uint8_t> bytes)
{
    std::ofstream file(path, std::ios::binary | std::ios::trunc);
    file.write(reinterpret_cast<const char *>(bytes.data()),
               static_cast<std::streamsize>(bytes.size()));
    file.close();
    if (!file)
        throw std::runtime_error("Cannot write fixture");
}

inline std::vector<std::uint8_t> read_bytes(const std::filesystem::path &path)
{
    std::ifstream file(path, std::ios::binary);
    if (!file)
        throw std::runtime_error("Cannot read fixture");
    return {std::istreambuf_iterator<char>(file), {}};
}
} // namespace closed_circuit::test

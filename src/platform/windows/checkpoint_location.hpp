#pragma once
#include "platform/windows/profile_location.hpp"

namespace closed_circuit::windows
{
// Same private directory and single-writer contract as the profile. Only the
// path is resolved here; reading, saving and clearing stay with their owners.
[[nodiscard]] inline ProfileLocationResult
checkpoint_location_in(const std::filesystem::path &base)
{
    auto directory = private_directory_in(base);
    if (SUCCEEDED(directory.error))
        directory.path /= L"checkpoint.bin";
    return directory;
}

[[nodiscard]] inline ProfileLocationResult local_checkpoint_location()
{
    const auto base = local_profile_base();
    return FAILED(base.error) ? base : checkpoint_location_in(base.path);
}
} // namespace closed_circuit::windows

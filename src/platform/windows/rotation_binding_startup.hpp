#pragma once
#include "platform/windows/profile_location.hpp"
#include "platform/windows/rotation_binding_file.hpp"

namespace closed_circuit::windows
{
struct RotationBindingStartup
{
    RotationBinding binding;
    std::filesystem::path path;
    HRESULT error;
};
// Only reads the separate setting next to the already resolved profile.
// A read failure leaves R playable, preserving the bad file for inspection.
[[nodiscard]] inline RotationBindingStartup
rotation_binding_startup(const ProfileLocationResult &profile) noexcept
{
    if (FAILED(profile.error))
        return {{}, {}, profile.error};
    try
    {
        if (!profile.path.is_absolute())
            return {{}, {}, E_INVALIDARG};
        auto path = profile.path.parent_path() / L"rotation-key.bin";
        const auto read = read_rotation_binding_file(path);
        return {read.binding.value_or(RotationBinding{}), std::move(path),
                read.error == ERROR_FILE_NOT_FOUND
                    ? S_OK
                    : HRESULT_FROM_WIN32(read.error)};
    }
    catch (...)
    {
        return {{}, {}, E_FAIL};
    }
}
} // namespace closed_circuit::windows

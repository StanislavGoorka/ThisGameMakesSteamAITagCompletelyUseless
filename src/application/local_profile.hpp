#pragma once

#include <cstdint>

namespace closed_circuit
{
// Internal IDs, never display labels. Append new IDs through a versioned
// format change; do not reinterpret an existing bit.
enum class AchievementId : std::uint8_t
{
    first_delivery = 1,
    first_sector = 2,
    run_without_overdrive = 3,
    reconfigured_delivery = 4,
    completed_run = 5
};

struct LocalProfile
{
    std::uint16_t achievement_bits = 0;
    // Bits 0/1 reserve alternate configurations 2/3. Configuration 1 is
    // always available. This stores ownership, not gameplay bonuses.
    std::uint8_t variant_bits = 0;
    std::uint8_t master_volume = 100;
    bool muted = false;

    constexpr bool operator==(const LocalProfile &) const noexcept = default;
};

[[nodiscard]] constexpr bool
is_profile_valid(const LocalProfile &profile) noexcept
{
    return profile.achievement_bits <= 0x1f && profile.variant_bits <= 3 &&
           profile.master_volume <= 100;
}
} // namespace closed_circuit

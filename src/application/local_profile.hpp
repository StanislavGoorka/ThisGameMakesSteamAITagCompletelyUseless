#pragma once

#include <array>
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
    completed_run = 5,
    // ACHIEVEMENT_EXPANSION_PLAN IDs; unknown until listed in the catalog.
    swift_sector = 6,
    won_preinstalled_range = 7,
    won_charged_debt = 8,
    won_short_turn_limit = 9,
    won_lean_delivery = 10,
    won_heavy_overdrive_debt = 11,
    won_all_modifiers = 12
};

// Only achievements with an implemented predicate, in ascending order. An ID
// outside it is refused by unlock, sinks and candidate masks.
inline constexpr std::array achievement_catalog{
    AchievementId::first_delivery,         AchievementId::first_sector,
    AchievementId::run_without_overdrive,  AchievementId::reconfigured_delivery,
    AchievementId::completed_run,          AchievementId::swift_sector,
    AchievementId::won_preinstalled_range, AchievementId::won_charged_debt,
    AchievementId::won_short_turn_limit,   AchievementId::won_lean_delivery};

[[nodiscard]] constexpr std::uint16_t implemented_achievement_bits() noexcept
{
    std::uint16_t bits = 0;
    for (const auto id : achievement_catalog)
        bits = static_cast<std::uint16_t>(
            bits | (1u << (static_cast<unsigned>(id) - 1)));
    return bits;
}

// Format range of achievement_bits since profile schema 2 (bits 0..11).
// Which IDs may be unlocked is decided separately.
inline constexpr std::uint16_t profile_achievement_bits = 0x0fff;
// Schema 1 stored only bits 0..4.
inline constexpr std::uint16_t profile_legacy_achievement_bits = 0x001f;

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
    return (profile.achievement_bits & ~profile_achievement_bits) == 0 &&
           profile.variant_bits <= 3 && profile.master_volume <= 100;
}
} // namespace closed_circuit

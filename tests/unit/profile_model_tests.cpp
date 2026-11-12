#include "application/local_profile.hpp"
#include "application/profile_format.hpp"
using namespace closed_circuit;

static_assert(static_cast<unsigned>(AchievementId::first_delivery) == 1);
static_assert(static_cast<unsigned>(AchievementId::first_sector) == 2);
static_assert(static_cast<unsigned>(AchievementId::run_without_overdrive) == 3);
static_assert(static_cast<unsigned>(AchievementId::reconfigured_delivery) == 4);
static_assert(static_cast<unsigned>(AchievementId::completed_run) == 5);
static_assert(ProfileBytes{}.size() == 24 && profile_schema_version == 1);
static_assert(is_profile_valid(LocalProfile{}));

int main()
{
    const LocalProfile original;
    if (original.achievement_bits != 0 || original.variant_bits != 0 ||
        original.master_volume != 100 || original.muted)
        return 1;
    auto copy = original;
    copy.achievement_bits = 31;
    copy.variant_bits = 3;
    copy.master_volume = 0;
    copy.muted = true;
    if (!is_profile_valid(copy) || copy == original ||
        original != LocalProfile{})
        return 1;
    for (unsigned raw = 0; raw <= 65535; ++raw)
    {
        auto profile = original;
        profile.achievement_bits = static_cast<std::uint16_t>(raw);
        const auto before = profile;
        if (is_profile_valid(profile) != (raw <= 31) || profile != before)
            return 1;
    }
    for (unsigned raw = 0; raw <= 255; ++raw)
    {
        auto profile = original;
        profile.master_volume = static_cast<std::uint8_t>(raw);
        if (is_profile_valid(profile) != (raw <= 100))
            return 1;
        profile = original;
        profile.variant_bits = static_cast<std::uint8_t>(raw);
        if (is_profile_valid(profile) != (raw <= 3))
            return 1;
    }
    return 0;
}

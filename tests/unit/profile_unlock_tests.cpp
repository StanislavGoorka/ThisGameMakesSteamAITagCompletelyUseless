#include "application/profile_unlock.hpp"
#include <initializer_list>
using namespace closed_circuit;

int main()
{
    for (unsigned mask = 0; mask < 32; ++mask)
        for (unsigned raw = 0; raw < 256; ++raw)
            for (const auto settings :
                 {LocalProfile{}, LocalProfile{0, 3, 17, true}})
            {
                auto profile = settings;
                profile.achievement_bits = static_cast<std::uint16_t>(mask);
                const auto before = profile;
                const auto id = static_cast<AchievementId>(raw);
                const bool known = raw >= 1 && raw <= 5;
                const bool owned = known && (mask & (1u << (raw - 1))) != 0;
                if (is_achievement_unlocked(profile, id) != owned ||
                    profile != before)
                    return 1;
                const auto result = unlock_achievement(profile, id);
                auto expected = before;
                if (known)
                    expected.achievement_bits |=
                        static_cast<std::uint16_t>(1u << (raw - 1));
                if (profile != expected ||
                    result != (!known  ? ProfileUnlockResult::invalid
                               : owned ? ProfileUnlockResult::already_unlocked
                                       : ProfileUnlockResult::unlocked))
                    return 1;
                const auto again = unlock_achievement(profile, id);
                if (profile != expected ||
                    again != (known ? ProfileUnlockResult::already_unlocked
                                    : ProfileUnlockResult::invalid))
                    return 1;
            }
    for (const auto invalid :
         {LocalProfile{32}, LocalProfile{0, 4}, LocalProfile{0, 0, 101},
          LocalProfile{65535, 255, 255, true}})
        for (unsigned raw = 0; raw < 256; ++raw)
        {
            auto profile = invalid;
            const auto id = static_cast<AchievementId>(raw);
            if (is_achievement_unlocked(profile, id) ||
                unlock_achievement(profile, id) !=
                    ProfileUnlockResult::invalid ||
                profile != invalid)
                return 1;
        }
    return 0;
}

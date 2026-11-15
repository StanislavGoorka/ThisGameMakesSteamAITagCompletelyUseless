#include "../support/run_checks.hpp"
#include "core/difficulty_modifiers.hpp"
#include "core/run_start.hpp"

#include <bit>
#include <cstdint>
#include <iostream>
#include <limits>
#include <vector>

using namespace closed_circuit;

namespace
{
bool check(bool condition, const char *message)
{
    if (!condition)
        std::cerr << message << '\n';
    return condition;
}

// All three bits are implemented: every mask 0..7 is valid.
bool factory()
{
    const DifficultyModifiers empty{};
    if (!check(empty.bits() == 0 && implemented_difficulty_bits() == 7 &&
                   difficulty_modifier_catalog.size() == 3,
               "Default modifiers must be the empty mask"))
        return false;
    const auto short_limit = DifficultyModifiers::from_value(1);
    if (!check(
            short_limit && short_limit->bits() == 1 &&
                short_limit->contains(DifficultyModifier::short_turn_limit) &&
                !short_limit->contains(DifficultyModifier::lean_delivery) &&
                !short_limit->contains(
                    DifficultyModifier::heavy_overdrive_debt) &&
                *short_limit != empty,
            "Bit1 must parse as short_turn_limit only"))
        return false;
    const auto lean = DifficultyModifiers::from_value(2);
    const auto both = DifficultyModifiers::from_value(3);
    if (!check(lean && lean->contains(DifficultyModifier::lean_delivery) &&
                   !lean->contains(DifficultyModifier::short_turn_limit) &&
                   both &&
                   both->contains(DifficultyModifier::short_turn_limit) &&
                   both->contains(DifficultyModifier::lean_delivery) &&
                   !both->contains(DifficultyModifier::heavy_overdrive_debt),
               "Bits 1 and 2 must combine independently"))
        return false;
    const auto heavy = DifficultyModifiers::from_value(4);
    const auto all = DifficultyModifiers::from_value(7);
    if (!check(heavy &&
                   heavy->contains(DifficultyModifier::heavy_overdrive_debt) &&
                   !heavy->contains(DifficultyModifier::short_turn_limit) &&
                   !heavy->contains(DifficultyModifier::lean_delivery) && all &&
                   all->contains(DifficultyModifier::short_turn_limit) &&
                   all->contains(DifficultyModifier::lean_delivery) &&
                   all->contains(DifficultyModifier::heavy_overdrive_debt),
               "Bit4 must combine with the other bits"))
        return false;
    for (const auto modifier : {DifficultyModifier::short_turn_limit,
                                DifficultyModifier::lean_delivery,
                                DifficultyModifier::heavy_overdrive_debt})
        if (!check(!empty.contains(modifier),
                   "Empty mask must contain no modifier"))
            return false;
    for (int value = -300; value <= 300; ++value)
    {
        const auto parsed = DifficultyModifiers::from_value(value);
        if (!check(parsed.has_value() == (value >= 0 && value <= 7),
                   "Only masks 0..7 are implemented"))
            return false;
    }
    for (const auto value : {8, 9, 15, 16, 0xFF})
        if (!check(!DifficultyModifiers::from_value(value),
                   "Bits beyond the three modifiers stay unknown"))
            return false;
    return check(!DifficultyModifiers::from_value(
                     std::numeric_limits<std::int64_t>::min()) &&
                     !DifficultyModifiers::from_value(
                         std::numeric_limits<std::uint64_t>::max()) &&
                     DifficultyModifiers::from_value(0U) == empty,
                 "Out-of-range values must be refused");
}

bool starts()
{
    std::vector<RunSeed> seeds;
    for (std::uint64_t seed = 0; seed < 256; ++seed)
        seeds.push_back(RunSeed{seed});
    seeds.push_back(RunSeed{std::numeric_limits<std::uint64_t>::max()});
    for (const auto seed : seeds)
    {
        if (!check(start_run(seed).sector.difficulty == DifficultyModifiers{},
                   "A default run must have no modifier"))
            return false;
        for (const auto config : start_config_catalog)
        {
            const auto plain = start_run(seed, config);
            const auto masked = start_run(seed, config, DifficultyModifiers{});
            if (!check(plain && masked && test::same_run(*plain, *masked) &&
                           masked->sector.difficulty == DifficultyModifiers{},
                       "The empty mask must keep the configured start"))
                return false;
            // A forged bit is stored as given and changes nothing else.
            auto forged =
                *start_run(seed, config,
                           std::bit_cast<DifficultyModifiers>(std::uint8_t{4}));
            if (!check(forged.sector.difficulty.bits() == 4 &&
                           !test::same_run(forged, *plain),
                       "start_run must store the modifiers"))
                return false;
            forged.sector.difficulty = DifficultyModifiers{};
            if (!check(test::same_run(forged, *plain),
                       "Modifiers must not change the generated sector"))
                return false;
        }
    }
    return check(!start_run(RunSeed{0}, static_cast<StartConfigId>(4),
                            DifficultyModifiers{}),
                 "An unknown configuration still creates no run");
}
} // namespace

int main()
{
    return factory() && starts() ? 0 : 1;
}

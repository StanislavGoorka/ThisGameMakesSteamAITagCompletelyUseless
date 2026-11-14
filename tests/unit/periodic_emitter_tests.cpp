#include "core/periodic_emitter.hpp"
#include <array>
#include <iostream>
#include <type_traits>
using namespace closed_circuit;
static_assert(!std::is_default_constructible_v<PeriodicEmitter>);
static_assert(!std::is_aggregate_v<PeriodicEmitter>);
namespace
{
constexpr auto id = *EntityId::from_value(UINT32_MAX);
constexpr auto position = *NodeId::from_value(48);
auto make(std::uint64_t period, std::uint64_t phase, std::uint64_t range = 1)
{
    return PeriodicEmitter::from_values(id, position, period, phase, range);
}
bool cases()
{
    for (std::uint64_t period = 0; period <= 256; ++period)
        for (std::uint64_t phase = 0; phase <= 256; ++phase)
        {
            const auto value = make(period, phase);
            const bool valid = period >= 2 && period <= 48 && phase < period;
            if (value.has_value() != valid)
                return false;
            if (value && (value->id() != id || value->position() != position ||
                          value->period() != period ||
                          value->phase() != phase || value->range() != 1))
                return false;
        }
    for (std::uint64_t range = 0; range <= 256; ++range)
        if (make(3, 0, range).has_value() != (range >= 1 && range <= 48))
            return false;
    for (const auto large :
         {std::uint64_t{65536}, std::uint64_t{UINT32_MAX}, UINT64_MAX})
        if (make(large, 0) || make(3, large) || make(3, 0, large))
            return false;
    const std::array<std::array<bool, 6>, 3> phases{
        {{false, false, true, false, false, true},
         {true, false, false, true, false, false},
         {false, true, false, false, true, false}}};
    for (unsigned phase = 0; phase < 3; ++phase)
        for (unsigned turn = 0; turn < 6; ++turn)
            if (make(3, phase)->fires_next(TurnIndex{turn}) !=
                phases[phase][turn])
                return false;
    // Independent widened arithmetic covers every valid schedule, many cycles,
    // and the final uint32 values. No iteration advances or mutates the value.
    for (unsigned period = 2; period <= 48; ++period)
        for (unsigned phase = 0; phase < period; ++phase)
        {
            const auto value = *make(period, phase, 48);
            const auto before = value;
            for (unsigned turn = 0; turn < 1024; ++turn)
                if (value.fires_next(TurnIndex{turn}) !=
                    ((std::uint64_t{turn} + 1) % period == phase))
                    return false;
            for (const auto turn : {UINT32_MAX - 1, UINT32_MAX})
                if (value.fires_next(TurnIndex{turn}) !=
                    ((std::uint64_t{turn} + 1) % period == phase))
                    return false;
            if (value != before)
                return false;
        }
    if (!make(2, 0)->fires_next(TurnIndex{UINT32_MAX}) ||
        !make(3, 1)->fires_next(TurnIndex{UINT32_MAX}) ||
        !make(48, 16)->fires_next(TurnIndex{UINT32_MAX}))
        return false;
    const auto original = *make(3, 0, 2);
    auto copy = original;
    copy = *make(4, 1, 48);
    return original == *make(3, 0, 2) && copy != original;
}
} // namespace
int main()
{
    if (!cases())
    {
        std::cerr << "Periodic emitter configuration or schedule mismatch\n";
        return 1;
    }
    return 0;
}

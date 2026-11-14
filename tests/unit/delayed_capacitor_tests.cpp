#include "core/delayed_capacitor.hpp"
#include <iostream>
#include <type_traits>
using namespace closed_circuit;
namespace
{
constexpr auto id = *EntityId::from_value(UINT32_MAX);
constexpr auto position = *NodeId::from_value(48);
static_assert(!std::is_default_constructible_v<DelayedCapacitor>);
constexpr bool compile_time_fuse()
{
    auto value = *DelayedCapacitor::from_values(id, position, 1, 1);
    return value.advance_fuse() && value.remaining() == 0 &&
           !value.advance_fuse();
}
static_assert(compile_time_fuse());
bool cases()
{
    for (const std::uint64_t bad :
         {0ull, 49ull, 256ull, 257ull, 4294967297ull, UINT64_MAX})
    {
        if (DelayedCapacitor::from_values(id, position, bad, 1) ||
            DelayedCapacitor::from_values(id, position, 1, bad))
            return false;
    }
    for (unsigned delay = 1; delay <= 48; ++delay)
        for (unsigned range = 1; range <= 48; ++range)
        {
            const auto initial =
                DelayedCapacitor::from_values(id, position, delay, range);
            if (!initial || initial->remaining() != delay)
                return false;
            auto value = *initial;
            auto copy = value;
            for (unsigned tick = 1; tick <= 96; ++tick)
            {
                const bool signal = value.advance_fuse();
                const unsigned expected = tick < delay ? delay - tick : 0;
                if (signal != (tick == delay) ||
                    value.remaining() != expected || value.id() != id ||
                    value.position() != position || value.delay() != delay ||
                    value.range() != range || initial->remaining() != delay ||
                    copy.remaining() != delay)
                    return false;
            }
            auto second = copy;
            static_cast<void>(copy.advance_fuse());
            if (second != *initial || second.remaining() != delay ||
                copy == second)
                return false;
            for (unsigned i = 0; i < delay; ++i)
                static_cast<void>(second.advance_fuse());
            if (second != value || initial->remaining() != delay)
                return false;
        }
    // Factory preserves both endpoint IDs and positions as independent values.
    const auto zero = *DelayedCapacitor::from_values(
        *EntityId::from_value(0), *NodeId::from_value(0), 48, 48);
    return zero.id().value() == 0 && zero.position().value() == 0 &&
           zero.delay() == 48 && zero.range() == 48;
}
} // namespace
int main()
{
    if (!cases())
    {
        std::cerr << "Delayed capacitor value/fuse mismatch\n";
        return 1;
    }
    return 0;
}

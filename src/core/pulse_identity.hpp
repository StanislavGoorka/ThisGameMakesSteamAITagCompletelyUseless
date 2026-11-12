#pragma once

#include "core/game_state.hpp"

namespace closed_circuit
{

// Valid boards have at most 49 pulses, so this bounded search is small.
// A wider candidate also guards synthetic collections spanning all EntityIds.
[[nodiscard]] constexpr std::optional<EntityId>
available_pulse_id(const GameState &state) noexcept
{
    std::uint64_t candidate = 0;
    while (candidate <= EntityId::max_value)
    {
        const bool used =
            std::ranges::any_of(state.pulses, [candidate](const Pulse &pulse)
                                { return pulse.id.value() == candidate; });
        if (!used)
        {
            return EntityId::from_value(candidate);
        }
        ++candidate;
    }
    return std::nullopt;
}

} // namespace closed_circuit

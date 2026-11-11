#pragma once

#include "core/pulse_chase.hpp"

#include <algorithm>
#include <vector>

namespace closed_circuit
{

// Each pulse acts once, in ID order, using occupancy after earlier moves.
// Storage order is preserved; contacts and turn cost belong to later steps.
inline void advance_pulses(GameState &state)
{
    std::vector<EntityId> order;
    order.reserve(state.pulses.size());
    for (const auto &pulse : state.pulses)
    {
        order.push_back(pulse.id);
    }
    std::ranges::sort(order);

    for (const auto id : order)
    {
        if (const auto next = next_pulse_step(state, id))
        {
            std::ranges::find(state.pulses, id, &Pulse::id)->position = *next;
        }
    }
}

} // namespace closed_circuit

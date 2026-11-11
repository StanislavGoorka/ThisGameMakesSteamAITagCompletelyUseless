#pragma once

#include "core/pulse_chase.hpp"
#include "core/pulse_contact.hpp"

#include <algorithm>
#include <vector>

namespace closed_circuit
{

// One phase shares one damage limit. Existing contacts are removed before
// any movement; new contacts free their node before the next ID acts.
inline void resolve_pulse_phase(GameState &state)
{
    bool damage_applied = false;
    std::vector<EntityId> order;
    order.reserve(state.pulses.size());
    for (const auto &pulse : state.pulses)
    {
        order.push_back(pulse.id);
    }
    std::ranges::sort(order);

    for (const auto id : order)
    {
        static_cast<void>(resolve_pulse_contact(state, id, damage_applied));
    }
    for (const auto id : order)
    {
        if (const auto next = next_pulse_step(state, id))
        {
            std::ranges::find(state.pulses, id, &Pulse::id)->position = *next;
            static_cast<void>(resolve_pulse_contact(state, id, damage_applied));
        }
    }
}

} // namespace closed_circuit

#pragma once

#include "core/final_sector_topology.hpp"

namespace closed_circuit
{
enum class SectorVariant : std::uint8_t
{
    normal,
    final
};

// Only local initial state. Inherited resources and paid debt pulses belong
// to the caller, so restore never accidentally repeats the transition charge.
[[nodiscard]] inline std::optional<GameState>
build_sector_state(RunSeed sector_seed, SectorVariant variant)
{
    const auto assemble = [](const auto &topology)
    {
        auto state = *GameState::initial(
            topology.ports, topology.rotatable_nodes, topology.entry);
        state.sector_entry = topology.entry;
        state.sector_exit = topology.exit;
        for (const auto receiver : topology.receivers)
            state.receiver_nodes[receiver.value()] = true;
        state.delivery_requirement =
            *DeliveryRequirement::from_value(topology.receivers.size());
        return state;
    };
    switch (variant)
    {
    case SectorVariant::normal:
        return assemble(generate_sector_topology(sector_seed));
    case SectorVariant::final:
        return assemble(generate_final_sector_topology(sector_seed));
    }
    return std::nullopt;
}
} // namespace closed_circuit

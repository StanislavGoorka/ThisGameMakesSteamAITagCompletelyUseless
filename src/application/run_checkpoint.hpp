#pragma once

#include "core/run_state.hpp"
#include "core/sector_topology.hpp"

namespace closed_circuit
{
struct RunCheckpoint
{
    RunSeed seed;
    SectorNumber sector_number;
    std::uint8_t player_integrity;
    std::uint8_t reserve;
    std::uint32_t overload_debt;
    ModuleLoadout modules;
    bool entry_debt_pulse;

    constexpr bool operator==(const RunCheckpoint &) const noexcept = default;
};

[[nodiscard]] constexpr bool
is_checkpoint_valid(const RunCheckpoint &checkpoint) noexcept
{
    return checkpoint.player_integrity >= 1 &&
           checkpoint.player_integrity <= GameState::initial_player_integrity &&
           checkpoint.reserve <= GameState::reserve_limit &&
           (checkpoint.entry_debt_pulse
                ? checkpoint.overload_debt <= UINT32_MAX - 3
                : checkpoint.overload_debt < 3);
}

[[nodiscard]] inline std::optional<RunState>
restore_checkpoint(const RunCheckpoint &checkpoint)
{
    if (!is_checkpoint_valid(checkpoint))
        return std::nullopt;
    SplitMix64 rng{checkpoint.seed};
    RunSeed sector_seed{0};
    for (unsigned i = 0; i < checkpoint.sector_number.value(); ++i)
        sector_seed = RunSeed{rng.next_u64()};
    const auto topology = generate_sector_topology(sector_seed);
    auto state = *GameState::initial(topology.ports, topology.rotatable_nodes,
                                     topology.entry);
    state.sector_entry = topology.entry;
    state.sector_exit = topology.exit;
    for (const auto receiver : topology.receivers)
        state.receiver_nodes[receiver.value()] = true;
    state.player_integrity = checkpoint.player_integrity;
    state.reserve = checkpoint.reserve;
    state.overload_debt = checkpoint.overload_debt;
    state.modules = checkpoint.modules;
    // This pulse was already paid for. Never charge debt again on resume.
    if (checkpoint.entry_debt_pulse)
        state.pulses.push_back({*EntityId::from_value(0), topology.entry});
    return RunState{checkpoint.seed, checkpoint.sector_number, std::move(state),
                    false};
}

[[nodiscard]] inline std::optional<RunCheckpoint>
capture_checkpoint(const RunState &run)
{
    RunCheckpoint checkpoint{run.seed,
                             run.sector_number,
                             run.sector.player_integrity,
                             run.sector.reserve,
                             run.sector.overload_debt,
                             run.sector.modules,
                             !run.sector.pulses.empty()};
    const auto restored = restore_checkpoint(checkpoint);
    if (!restored)
        return std::nullopt;
    const auto &a = run.sector;
    const auto &b = restored->sector;
    // Compare every field: a boundary projection must never silently lose data.
    if (run.reward_claimed != restored->reward_claimed ||
        a.player_position != b.player_position ||
        a.turns_taken != b.turns_taken || a.packet != b.packet ||
        a.powered_nodes != b.powered_nodes ||
        a.receiver_nodes != b.receiver_nodes ||
        a.sector_entry != b.sector_entry || a.sector_exit != b.sector_exit ||
        a.pulses != b.pulses || a.blocked_nodes != b.blocked_nodes ||
        a.rotatable_nodes != b.rotatable_nodes ||
        a.terminal_rotation_used != b.terminal_rotation_used ||
        a.player_integrity != b.player_integrity || a.reserve != b.reserve ||
        a.overload_debt != b.overload_debt || a.modules != b.modules ||
        !std::ranges::equal(a.ports, b.ports, {}, &PortMask::bits,
                            &PortMask::bits))
        return std::nullopt;
    return checkpoint;
}
} // namespace closed_circuit

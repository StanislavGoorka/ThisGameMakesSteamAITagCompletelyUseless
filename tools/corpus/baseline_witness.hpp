#pragma once

#include "core/solution_certificate.hpp"
#include "corpus/sector_fixture.hpp"

namespace closed_circuit::developer
{
// Only the exact baseline for this seed is supported. This is a developer
// witness for P028's two perimeter paths, never a general solver or filter.
[[nodiscard]] inline std::optional<std::vector<Command>>
build_baseline_witness(RunSeed seed, const GameState &initial)
{
    const auto expected = sector_fixture(seed);
    if (!std::ranges::equal(initial.ports, expected.ports, {}, &PortMask::bits,
                            &PortMask::bits) ||
        initial.rotatable_nodes != expected.rotatable_nodes ||
        initial.blocked_nodes != expected.blocked_nodes ||
        initial.player_position != expected.player_position ||
        initial.player_integrity != expected.player_integrity ||
        initial.turns_taken != expected.turns_taken ||
        initial.packet != expected.packet ||
        initial.powered_nodes != expected.powered_nodes ||
        initial.receiver_nodes != expected.receiver_nodes ||
        initial.sector_entry != expected.sector_entry ||
        initial.sector_exit != expected.sector_exit ||
        initial.pulses != expected.pulses ||
        initial.reserve != expected.reserve ||
        initial.overload_debt != expected.overload_debt ||
        initial.modules != expected.modules ||
        initial.terminal_rotation_used != expected.terminal_rotation_used)
    {
        return std::nullopt;
    }
    auto state = initial;
    std::vector<Command> commands;
    const bool top = initial.receiver_nodes[4];
    const auto at = [top](int i)
    {
        return top ? (i <= 6 ? i : 6 + (i - 6) * 7)
                   : (i <= 6 ? i * 7 : 42 + i - 6);
    };
    const auto direction = [](int from, int to) {
        return to == from + 1 ? 2 : to == from - 1 ? 8 : to > from ? 4 : 1;
    };
    const auto apply = [&](Command command)
    {
        auto result = resolve_turn(state, command);
        if (!result.accepted)
            return false;
        commands.push_back(command);
        state = std::move(result.state);
        return true;
    };
    for (int i = 1; i < 13; ++i)
    {
        const int n = at(i);
        const auto target = *NodeId::from_value(n);
        const int desired =
            direction(n, at(i - 1)) | (i < 12 ? direction(n, at(i + 1)) : 0);
        int turns = 0;
        while (state.ports[n].bits() != desired && turns < 4)
        {
            if (!apply(RotateCommand{target}))
                return std::nullopt;
            ++turns;
        }
        if (state.ports[n].bits() != desired || !apply(MoveCommand{target}))
            return std::nullopt;
    }
    while (sector_outcome(state) == SectorOutcome::ongoing &&
           commands.size() < 48)
        if (!apply(WaitCommand{}))
            return std::nullopt;
    if (sector_outcome(state) != SectorOutcome::completed)
        return std::nullopt;
    return commands;
}
} // namespace closed_circuit::developer

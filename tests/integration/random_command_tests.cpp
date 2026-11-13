#include "../support/state_checks.hpp"
#include "core/debt_spawn.hpp"
#include "core/turn_preview.hpp"
#include "corpus/sector_fixture.hpp"
#include <bit>
#include <iostream>
using namespace closed_circuit;

std::vector<Command> commands()
{
    std::vector<Command> result{WaitCommand{}};
    for (unsigned n = 0; n < GameState::node_count; ++n)
    {
        const auto id = *NodeId::from_value(n);
        result.push_back(MoveCommand{id});
        result.push_back(RotateCommand{id});
        result.push_back(OverdriveCommand{id});
    }
    return result;
}

bool invariant(const GameState &before, const TurnResult &result)
{
    const auto &after = result.state;
    if (!result.accepted)
        return test::same_state(before, after);
    if (after.player_integrity > before.player_integrity ||
        before.player_integrity - after.player_integrity > 1 ||
        static_cast<std::uint32_t>(after.turns_taken) !=
            static_cast<std::uint32_t>(before.turns_taken) + 1 ||
        after.reserve > GameState::reserve_limit ||
        after.receiver_nodes != before.receiver_nodes ||
        after.rotatable_nodes != before.rotatable_nodes ||
        after.blocked_nodes != before.blocked_nodes ||
        after.modules != before.modules ||
        after.sector_entry != before.sector_entry ||
        after.sector_exit != before.sector_exit)
        return false;
    for (std::size_t n = 0; n < before.ports.size(); ++n)
        if (after.ports[n].bits() > 15 ||
            std::popcount(after.ports[n].bits()) !=
                std::popcount(before.ports[n].bits()) ||
            (before.powered_nodes[n] && !after.powered_nodes[n]))
            return false;
    for (std::size_t i = 0; i < after.pulses.size(); ++i)
    {
        const auto &pulse = after.pulses[i];
        if (std::ranges::none_of(before.pulses, [&](const auto &p)
                                 { return p.id == pulse.id; }))
            return false;
        for (std::size_t j = 0; j < i; ++j)
            if (after.pulses[j].id == pulse.id ||
                after.pulses[j].position == pulse.position)
                return false;
    }
    return true;
}

void reproduce(std::uint64_t seed, unsigned family,
               const std::vector<Command> &trace)
{
    std::cerr << "seed " << seed << " family " << family << " modules "
              << (seed % 4) << " commands";
    for (const auto &command : trace)
        std::visit(
            [&](const auto &action)
            {
                std::cerr << ' ' << command.index();
                if constexpr (requires { action.target; })
                    std::cerr << ':' << unsigned(action.target.value());
            },
            command);
    std::cerr << '\n';
}

int main()
{
    const auto all = commands();
    std::array<unsigned, 4> accepted{}, rejected{};
    for (unsigned index = 0; index < 64; ++index)
        for (unsigned family = 0; family < 2; ++family)
        {
            const std::uint64_t seed = index == 63 ? UINT64_MAX : index;
            auto state = developer::sector_fixture(RunSeed{seed});
            if (seed % 4 & 1)
                static_cast<void>(state.modules.add(ModuleId::rotation_range));
            if (seed % 4 & 2)
                static_cast<void>(
                    state.modules.add(ModuleId::terminal_rotation));
            if (family == 1)
            {
                state.overload_debt = 6;
                if (!spawn_debt_pulse(state))
                    return 2;
                state.pulses.push_back(
                    {*EntityId::from_value(99), *NodeId::from_value(48)});
            }
            auto replay = state;
            SplitMix64 rng(RunSeed{seed ^ (0x9e3779b97f4a7c15ull * family)});
            std::vector<Command> trace;
            for (unsigned step = 0; step < 128; ++step)
            {
                std::vector<Command> pool;
                const bool legal = step % 2 == 0;
                for (const auto &command : all)
                    if (is_command_legal(state, command) == legal)
                        pool.push_back(command);
                if (pool.empty())
                    pool = all; // Terminal states exercise atomic rejection.
                const auto command =
                    pool[*rng.next_in_range(0, pool.size() - 1)];
                trace.push_back(command);
                const auto before = state;
                const auto preview = preview_turn(state, command);
                const auto result = resolve_turn(state, command);
                const auto repeated = resolve_turn(replay, command);
                if (result.accepted != is_command_legal(state, command) ||
                    preview.accepted != result.accepted ||
                    repeated.accepted != result.accepted ||
                    preview.outcome != result.outcome ||
                    repeated.outcome != result.outcome ||
                    !test::same_state(before, state) ||
                    !test::same_state(preview.state, result.state) ||
                    !test::same_state(repeated.state, result.state) ||
                    !invariant(before, result))
                {
                    reproduce(seed, family, trace);
                    return 1;
                }
                (result.accepted ? accepted : rejected)[command.index()]++;
                state = result.state;
                replay = repeated.state;
            }
        }
    for (std::size_t kind = 0; kind < 4; ++kind)
    {
        if (!accepted[kind] || !rejected[kind])
            return 3;
        std::cout << "kind " << kind << " accepted " << accepted[kind]
                  << " rejected " << rejected[kind] << '\n';
    }
    std::cout << "PASS: 64 seeds, 2 families, 128 commands each\n";
}

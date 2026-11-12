#include "core/grid_pathfinding.hpp"
#include "core/turn_preview.hpp"

#include <algorithm>
#include <array>
#include <iostream>
#include <string_view>
#include <vector>

using namespace closed_circuit;

namespace
{

constexpr NodeId node(int value)
{
    return *NodeId::from_value(value);
}

constexpr auto pulse_id = *EntityId::from_value(1);

GameState initial_scenario()
{
    GameState state{.player_position = node(0),
                    .packet = Packet{node(7), node(8)},
                    .sector_exit = node(48),
                    .pulses = {{pulse_id, node(8)}}};
    state.ports[0] = *PortMask::from_bits(PortMask::south_bit);
    state.ports[7] =
        *PortMask::from_bits(PortMask::north_bit | PortMask::west_bit);
    state.ports[8] = *PortMask::from_bits(PortMask::west_bit);
    state.rotatable_nodes[7] = true;
    state.receiver_nodes[8] = true;
    state.receiver_nodes[48] = true;
    state.reserve = 0;
    state.overload_debt = 17;
    return state;
}

bool same_state(const GameState &left, const GameState &right)
{
    return left.reserve == right.reserve &&
           left.overload_debt == right.overload_debt &&
           left.player_position == right.player_position &&
           left.player_integrity == right.player_integrity &&
           left.turns_taken == right.turns_taken &&
           left.packet == right.packet &&
           left.powered_nodes == right.powered_nodes &&
           left.receiver_nodes == right.receiver_nodes &&
           left.sector_entry == right.sector_entry &&
           left.sector_exit == right.sector_exit &&
           left.pulses == right.pulses &&
           left.rotatable_nodes == right.rotatable_nodes &&
           left.blocked_nodes == right.blocked_nodes &&
           std::ranges::equal(left.ports, right.ports, {}, &PortMask::bits,
                              &PortMask::bits);
}

using Sequence = std::array<Command, 3>;
using Snapshots = std::array<GameState, 3>;

bool run_sequence(const GameState &initial, const Sequence &commands,
                  const Snapshots &expected, std::string_view name)
{
    const auto initial_before = initial;
    const auto commands_before = commands;
    auto live = initial;
    for (std::size_t index = 0; index < commands.size(); ++index)
    {
        const auto before = live;
        const auto preview = preview_turn(live, commands[index]);
        const auto repeated = preview_turn(live, commands[index]);
        const auto committed = resolve_turn(live, commands[index]);
        if (!same_state(live, before) || !same_state(initial, initial_before) ||
            commands != commands_before)
        {
            std::cerr << name << ": input state or commands changed\n";
            return false;
        }
        for (const auto *result : {&preview, &repeated, &committed})
        {
            if (!result->accepted ||
                result->outcome != SectorOutcome::ongoing ||
                !same_state(result->state, expected[index]))
            {
                std::cerr << name << ": unexpected full state on turn "
                          << index + 1 << '\n';
                return false;
            }
        }
        live = committed.state;
        const auto deliveries = std::ranges::count(live.powered_nodes, true);
        std::cout << name << ' ' << static_cast<std::uint32_t>(live.turns_taken)
                  << ' ' << static_cast<unsigned>(live.ports[7].bits()) << ' '
                  << (live.packet ? live.packet->position.value() : -1) << ' '
                  << (live.pulses.empty()
                          ? -1
                          : static_cast<int>(live.pulses[0].position.value()))
                  << ' ' << deliveries << ' '
                  << static_cast<unsigned>(live.player_integrity) << '\n';
    }
    return true;
}

} // namespace

int main()
{
    const auto initial = initial_scenario();
    Snapshots waiting{initial, initial, initial};
    for (std::size_t index = 0; index < waiting.size(); ++index)
    {
        waiting[index].turns_taken = static_cast<TurnIndex>(index + 1);
    }

    auto opened = initial;
    opened.ports[7] =
        *PortMask::from_bits(PortMask::north_bit | PortMask::east_bit);
    opened.packet.reset();
    opened.powered_nodes[8] = true;
    opened.reserve = 1;
    opened.pulses[0].position = node(7);
    opened.turns_taken = TurnIndex{1};

    Snapshots exposed{opened, opened, opened};
    for (std::size_t index = 1; index < exposed.size(); ++index)
    {
        exposed[index].pulses.clear();
        exposed[index].player_integrity = 2;
        exposed[index].turns_taken = static_cast<TurnIndex>(index + 1);
    }

    Snapshots protected_route{opened, opened, opened};
    for (std::size_t index = 1; index < protected_route.size(); ++index)
    {
        protected_route[index].ports[7] =
            *PortMask::from_bits(PortMask::east_bit | PortMask::south_bit);
        protected_route[index].turns_taken = static_cast<TurnIndex>(index + 1);
    }

    if (!shortest_route(initial.ports, node(7), node(8)).empty() ||
        !shortest_route(initial.ports, node(8), node(0)).empty() ||
        shortest_route(opened.ports, node(7), node(8)) !=
            std::vector{node(7), node(8)} ||
        shortest_route(opened.ports, node(8), node(0)) !=
            std::vector{node(8), node(7), node(0)} ||
        !shortest_route(protected_route[1].ports, node(7), node(0)).empty())
    {
        std::cerr << "Shared-edge route expectations failed\n";
        return 1;
    }

    const Sequence wait{WaitCommand{}, WaitCommand{}, WaitCommand{}};
    const Sequence open{RotateCommand{node(7)}, WaitCommand{}, WaitCommand{}};
    const Sequence close{RotateCommand{node(7)}, RotateCommand{node(7)},
                         WaitCommand{}};
    if (!run_sequence(initial, wait, waiting, "wait") ||
        !run_sequence(initial, open, exposed, "open") ||
        !run_sequence(initial, close, protected_route, "open_close"))
    {
        return 1;
    }
    return 0;
}

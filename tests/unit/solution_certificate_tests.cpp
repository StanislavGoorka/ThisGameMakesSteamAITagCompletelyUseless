#include "core/grid_pathfinding.hpp"
#include "core/sector_topology.hpp"
#include "core/solution_certificate.hpp"
#include <algorithm>
#include <iostream>
using namespace closed_circuit;
namespace
{
NodeId node(int n)
{
    return *NodeId::from_value(n);
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

GameState corridor()
{
    GameState s{.player_position = node(0)};
    s.ports[0] = *PortMask::from_bits(2);
    s.ports[1] = *PortMask::from_bits(12);
    s.ports[8] = *PortMask::from_bits(1);
    s.sector_entry = node(0);
    s.sector_exit = node(8);
    s.receiver_nodes[1] = s.receiver_nodes[8] = true;
    s.reserve = 1;
    s.overload_debt = 17;
    s.pulses = {{*EntityId::from_value(UINT32_MAX), node(48)}};
    return s;
}
bool expect(const GameState &s, const std::vector<Command> &commands,
            bool accepted)
{
    const auto before = s;
    const auto before_commands = commands;
    SplitMix64 rng{RunSeed{7}};
    auto reference = rng;
    for (int repeat = 0; repeat < 3; ++repeat)
        if (validate_solution(s, commands) != accepted ||
            !same_state(s, before) || commands != before_commands ||
            rng.next_u64() != reference.next_u64())
        {
            std::cerr << "Certificate result or input immutability mismatch\n";
            return false;
        }
    return true;
}
bool corridor_certificates()
{
    const auto s = corridor();
    const std::vector<Command> winning{WaitCommand{}, WaitCommand{},
                                       MoveCommand{node(1)},
                                       MoveCommand{node(8)}};
    if (!expect(s, winning, true))
        return false;
    auto end = s;
    for (const auto &c : winning)
        end = resolve_turn(end, c).state;
    auto explicit_end = s;
    explicit_end.player_position = node(8);
    explicit_end.powered_nodes[1] = explicit_end.powered_nodes[8] = true;
    explicit_end.reserve = 3;
    explicit_end.turns_taken = TurnIndex{4};
    if (!same_state(end, explicit_end) ||
        sector_outcome(end) != SectorOutcome::completed)
        return false;
    auto extra = winning;
    extra.push_back(WaitCommand{});
    auto cut = winning;
    cut.pop_back();
    if (shortest_route(s.ports, node(0), node(8)).empty() ||
        !expect(s, {}, false) ||
        !expect(s, {MoveCommand{node(1)}, MoveCommand{node(8)}}, false) ||
        !expect(s, {MoveCommand{node(48)}}, false) ||
        !expect(s, extra, false) || !expect(s, cut, false))
        return false;
    auto death = s;
    death.player_integrity = 1;
    death.pulses.push_back({*EntityId::from_value(0), node(0)});
    if (!expect(death, winning, false))
        return false;
    auto limit = s;
    limit.turns_taken = TurnIndex{47};
    if (!expect(limit, winning, false))
        return false;
    auto last = s;
    last.turns_taken = TurnIndex{47};
    last.player_position = node(1);
    last.powered_nodes[1] = last.powered_nodes[8] = true;
    if (!expect(last, {MoveCommand{node(8)}}, true))
        return false;
    auto fatal = last;
    fatal.player_integrity = 1;
    fatal.pulses.push_back({*EntityId::from_value(0), node(8)});
    if (!expect(fatal, {MoveCommand{node(8)}}, false))
        return false;
    for (int mode = 0; mode < 3; ++mode)
    {
        auto terminal = s;
        if (mode == 0)
            terminal.player_integrity = 0;
        if (mode == 1)
            terminal.turns_taken = TurnIndex{48};
        if (mode == 2)
        {
            terminal = explicit_end;
        }
        if (!expect(terminal, {}, false) || !expect(terminal, winning, false))
            return false;
    }
    return true;
}
// Test-only witness construction for the constrained family, not a production
// solver or a generator acceptance filter.
bool generated_certificate(RunSeed seed)
{
    const auto t = generate_sector_topology(seed);
    auto s = *GameState::initial(t.ports, t.rotatable_nodes, t.entry);
    s.sector_entry = t.entry;
    s.sector_exit = t.exit;
    for (auto receiver : t.receivers)
        s.receiver_nodes[receiver.value()] = true;
    s.reserve = 1;
    s.overload_debt = 17;
    s.pulses = {{*EntityId::from_value(5), node(24)}};
    const auto initial = s;
    std::vector<Command> commands;
    const bool top = t.receivers[0] == node(4);
    const auto at = [top](int i)
    {
        return top ? (i <= 6 ? i : 6 + (i - 6) * 7)
                   : (i <= 6 ? i * 7 : 42 + i - 6);
    };
    const auto direction = [](int from, int to) {
        return to == from + 1 ? 2 : to == from - 1 ? 8 : to > from ? 4 : 1;
    };
    const auto apply = [&](Command c)
    {
        commands.push_back(c);
        auto r = resolve_turn(s, c);
        if (!r.accepted)
            return false;
        s = std::move(r.state);
        return true;
    };
    for (int i = 1; i < 13; ++i)
    {
        const int n = at(i);
        const int desired =
            direction(n, at(i - 1)) | (i < 12 ? direction(n, at(i + 1)) : 0);
        int turns = 0;
        while (s.ports[n].bits() != desired && turns < 4)
        {
            if (!apply(RotateCommand{node(n)}))
                return false;
            ++turns;
        }
        if (s.ports[n].bits() != desired || !apply(MoveCommand{node(n)}))
            return false;
    }
    while (sector_outcome(s) == SectorOutcome::ongoing && commands.size() < 48)
        if (!apply(WaitCommand{}))
            return false;
    if (sector_outcome(s) != SectorOutcome::completed ||
        s.player_position != node(48) || s.player_integrity != 3 ||
        s.reserve != 3 || s.overload_debt != 17 || s.packet ||
        !s.powered_nodes[t.receivers[0].value()] ||
        !s.powered_nodes[t.receivers[1].value()] ||
        s.pulses != initial.pulses ||
        static_cast<std::uint32_t>(s.turns_taken) != commands.size() ||
        commands.size() > 48)
    {
        std::cerr << "Generated witness did not complete actual play\n";
        return false;
    }
    if (!expect(initial, commands, true))
        return false;
    std::cout << static_cast<std::uint64_t>(seed) << " " << commands.size()
              << "\n";
    commands.pop_back();
    return expect(initial, commands, false);
}
} // namespace
int main()
{
    if (!corridor_certificates())
        return 1;
    for (const auto seed : {0ULL, 1ULL, 2ULL, UINT64_MAX})
        if (!generated_certificate(RunSeed{seed}))
            return 1;
    return 0;
}

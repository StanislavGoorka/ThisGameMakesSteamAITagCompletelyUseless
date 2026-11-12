#include "replay/state_hash.hpp"

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <string_view>
#include <utility>

using namespace closed_circuit;
using closed_circuit::developer::state_hash;

namespace
{

constexpr NodeId node(int value)
{
    return *NodeId::from_value(value);
}

constexpr EntityId entity(std::uint32_t value)
{
    return *EntityId::from_value(value);
}

bool same_state(const GameState &left, const GameState &right)
{
    return left.modules == right.modules && left.reserve == right.reserve &&
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

GameState full_state()
{
    GameState state{
        .player_position = node(48),
        .player_integrity = 255,
        .turns_taken = TurnIndex{UINT32_MAX},
        .packet = Packet{node(0), node(48)},
        .sector_entry = node(0),
        .sector_exit = node(48),
        .pulses = {{entity(UINT32_MAX), node(48)}, {entity(0), node(0)}}};
    for (std::size_t index = 0; index < GameState::node_count; ++index)
    {
        state.ports[index] = *PortMask::from_bits(index % 16);
        state.rotatable_nodes[index] = index % 2 == 0;
        state.blocked_nodes[index] = index % 3 == 0;
        state.powered_nodes[index] = index % 5 == 0;
        state.receiver_nodes[index] = index % 7 == 0;
    }
    return state;
}

bool unchanged = true;

std::uint64_t checked_hash(const GameState &state)
{
    const auto before = state;
    const auto hash = state_hash(state);
    if (!same_state(state, before))
    {
        std::cerr << "Hash query modified the complete state\n";
        unchanged = false;
    }
    return hash;
}

bool differs(const GameState &base, const GameState &changed,
             std::string_view field)
{
    if (checked_hash(base) == checked_hash(changed))
    {
        std::cerr << "Hash must distinguish changed " << field << '\n';
        return false;
    }
    return true;
}

bool arrays_are_encoded(const GameState &base)
{
    for (std::size_t index = 0; index < GameState::node_count; ++index)
    {
        auto changed = base;
        changed.ports[index] =
            *PortMask::from_bits(base.ports[index].bits() ^ 1);
        if (!differs(base, changed, "ports index"))
        {
            return false;
        }
        for (auto member :
             {&GameState::rotatable_nodes, &GameState::blocked_nodes,
              &GameState::powered_nodes, &GameState::receiver_nodes})
        {
            changed = base;
            (changed.*member)[index] = !(base.*member)[index];
            if (!differs(base, changed, "boolean array index"))
            {
                return false;
            }
        }
    }
    return true;
}

bool scalars_and_optionals_are_encoded(const GameState &base)
{
    auto changed = base;
    changed.player_position = node(0);
    if (!differs(base, changed, "player position"))
    {
        return false;
    }
    for (auto hp : {std::uint8_t{0}, std::uint8_t{3}})
    {
        changed = base;
        changed.player_integrity = hp;
        if (!differs(base, changed, "integrity"))
        {
            return false;
        }
    }
    for (auto turns : {TurnIndex{0}, TurnIndex{48}})
    {
        changed = base;
        changed.turns_taken = turns;
        if (!differs(base, changed, "turn count"))
        {
            return false;
        }
    }
    changed = base;
    changed.packet.reset();
    if (!differs(base, changed, "packet presence"))
    {
        return false;
    }
    changed = base;
    changed.packet->position = node(48);
    if (!differs(base, changed, "packet position"))
    {
        return false;
    }
    changed = base;
    changed.packet->target = node(0);
    if (!differs(base, changed, "packet target"))
    {
        return false;
    }
    for (auto member : {&GameState::sector_entry, &GameState::sector_exit})
    {
        auto absent = base;
        (absent.*member).reset();
        auto zero = base;
        zero.*member = node(0);
        auto last = base;
        last.*member = node(48);
        if (!differs(absent, zero, "optional absent versus zero") ||
            !differs(absent, last, "optional absent versus last") ||
            !differs(zero, last, "optional node value"))
        {
            return false;
        }
    }
    return true;
}

bool pulses_are_encoded(const GameState &base)
{
    for (std::size_t index = 0; index < base.pulses.size(); ++index)
    {
        auto changed = base;
        changed.pulses[index].id =
            entity(base.pulses[index].id.value() == 0 ? UINT32_MAX : 0);
        if (!differs(base, changed, "pulse ID"))
        {
            return false;
        }
        changed = base;
        changed.pulses[index].position =
            node(base.pulses[index].position.value() == 0 ? 48 : 0);
        if (!differs(base, changed, "pulse position"))
        {
            return false;
        }
    }
    auto changed = base;
    changed.pulses.clear();
    if (!differs(base, changed, "empty pulse collection"))
    {
        return false;
    }
    changed = base;
    changed.pulses.pop_back();
    if (!differs(base, changed, "pulse count"))
    {
        return false;
    }
    changed = base;
    std::swap(changed.pulses[0], changed.pulses[1]);
    return differs(base, changed, "stored pulse order");
}

} // namespace

int main()
{
    const GameState minimal{.player_position = node(0)};
    const auto full = full_state();
    // Independently constructed byte vectors, computed with PowerShell
    // BigInteger. See history/P031-02.md for the complete reference
    // calculation.
    if (checked_hash(minimal) != 15636418876360735861ULL)
    {
        std::cerr << "Minimal state must match canonical v3 vector\n";
        return 1;
    }
    if (checked_hash(full) != 12852721899160713969ULL)
    {
        std::cerr << "Full state must match canonical v3 vector\n";
        return 1;
    }
    struct ResourceVector
    {
        std::uint8_t reserve;
        std::uint32_t debt;
        std::uint64_t hash;
    };
    const ResourceVector resources[]{{0, 0, 5646037334929053375ULL},
                                     {1, 0, 9874218601067987940ULL},
                                     {2, 0, 15636418876360735861ULL},
                                     {3, 0, 1417856068790118810ULL},
                                     {255, 0, 9280530933878142310ULL},
                                     {2, 1, 18188160845316355644ULL},
                                     {2, UINT32_MAX, 10403108645413461041ULL}};
    for (const auto &vector : resources)
    {
        auto value = minimal;
        value.reserve = vector.reserve;
        value.overload_debt = vector.debt;
        if (checked_hash(value) != vector.hash)
        {
            std::cerr << "Resource vector mismatch\n";
            return 1;
        }
    }
    for (int repeat = 0; repeat < 3; ++repeat)
    {
        const auto copy = full;
        if (checked_hash(copy) != checked_hash(full))
        {
            std::cerr << "Copies and repeated hashes must agree\n";
            return 1;
        }
    }
    if (!arrays_are_encoded(full) || !scalars_and_optionals_are_encoded(full) ||
        !pulses_are_encoded(full))
    {
        return 1;
    }
    return unchanged ? 0 : 1;
}

#include "replay/seeded_fixture.hpp"
#include "replay/state_hash.hpp"

#include <algorithm>
#include <array>
#include <cstdint>
#include <iostream>

using namespace closed_circuit;
using closed_circuit::developer::seeded_fixture;
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
    return left.player_position == right.player_position &&
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

GameState expected_state(std::uint8_t node_one_mask)
{
    GameState expected{.player_position = node(0),
                       .player_integrity = 3,
                       .turns_taken = TurnIndex{0},
                       .packet = std::nullopt,
                       .sector_entry = node(0),
                       .sector_exit = node(8),
                       .pulses = {{entity(1), node(7)}}};
    // Direct numerical masks, independent of the production construction and
    // rotation loop. All other array entries retain their explicit zero state.
    expected.ports[0] = *PortMask::from_bits(6);
    expected.ports[1] = *PortMask::from_bits(node_one_mask);
    expected.ports[7] = *PortMask::from_bits(3);
    expected.ports[8] = *PortMask::from_bits(9);
    expected.rotatable_nodes[1] = true;
    expected.receiver_nodes[1] = true;
    expected.receiver_nodes[8] = true;
    return expected;
}

struct Vector
{
    RunSeed seed;
    std::uint8_t mask;
    std::uint64_t hash;
};

// Independent BigInteger SplitMix64 and canonical byte stream calculations;
// see history/P024-02.md. These four seeds cover every initial orientation.
constexpr std::array vectors{
    Vector{RunSeed{0}, 6, 7229561696845526205ULL},
    Vector{RunSeed{1}, 9, 18006929851103986098ULL},
    Vector{RunSeed{2}, 3, 826881297037723540ULL},
    Vector{RunSeed{UINT64_MAX}, 12, 18070937588695573379ULL}};

bool vectors_repeat_without_changing_external_rng()
{
    SplitMix64 external{RunSeed{42}};
    static_cast<void>(external.next_u64());
    auto reference = external;
    for (const auto &vector : vectors)
    {
        const auto expected = expected_state(vector.mask);
        for (int repeat = 0; repeat < 3; ++repeat)
        {
            const auto actual = seeded_fixture(vector.seed);
            if (!same_state(actual, expected) ||
                state_hash(actual) != vector.hash)
            {
                std::cerr
                    << "Fixture must match full independent seed vector\n";
                return false;
            }
            if (external.next_u64() != reference.next_u64())
            {
                std::cerr << "Fixture must leave external RNG untouched\n";
                return false;
            }
        }
    }
    return true;
}

bool values_are_independent()
{
    for (const auto &vector : vectors)
    {
        const auto before = seeded_fixture(vector.seed);
        auto changed = seeded_fixture(vector.seed);
        auto copy = changed;
        changed.ports[0] = *PortMask::from_bits(0);
        changed.rotatable_nodes[1] = false;
        changed.blocked_nodes[48] = true;
        changed.player_position = node(48);
        changed.player_integrity = 0;
        changed.turns_taken = TurnIndex{48};
        changed.packet = Packet{node(48), node(0)};
        changed.powered_nodes[1] = true;
        changed.receiver_nodes[8] = false;
        changed.sector_entry.reset();
        changed.sector_exit = node(48);
        changed.pulses[0].position = node(48);
        changed.pulses.push_back({entity(2), node(0)});
        const auto after = seeded_fixture(vector.seed);
        if (!same_state(before, after) || !same_state(copy, before) ||
            !same_state(after, expected_state(vector.mask)) ||
            state_hash(after) != vector.hash)
        {
            std::cerr
                << "Fixture values must own independent complete states\n";
            return false;
        }
        copy.pulses.clear();
        copy.sector_exit.reset();
        if (changed.pulses.size() != 2 || changed.sector_exit != node(48) ||
            !same_state(before, seeded_fixture(vector.seed)))
        {
            std::cerr << "Mutating a copy must preserve other fixture values\n";
            return false;
        }
    }
    return true;
}

} // namespace

int main()
{
    return vectors_repeat_without_changing_external_rng() &&
                   values_are_independent()
               ? 0
               : 1;
}

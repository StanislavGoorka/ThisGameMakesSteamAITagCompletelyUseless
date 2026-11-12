#include "../support/state_checks.hpp"
#include "corpus/fixed_seeds.hpp"
#include "corpus/sector_fixture.hpp"

#include <iostream>
#include <set>
using namespace closed_circuit;
using namespace closed_circuit::developer;
int main()
{
    const std::set<std::uint64_t> unique(sector_seeds.begin(),
                                         sector_seeds.end());
    if (sector_seeds.size() != 1000 || unique.size() != 1000 ||
        sector_seeds.front() != 0 || sector_seeds.back() != UINT64_MAX)
        return 1;
    for (std::size_t i = 0; i < 999; ++i)
        if (sector_seeds[i] != i)
            return 1;
    const std::array<std::uint8_t, 13> bottom{4, 5,  10, 5, 5,  10, 12,
                                              5, 10, 10, 5, 10, 8};
    const std::array<std::uint8_t, 13> top{2, 10, 5,  10, 10, 5, 6,
                                           5, 5,  10, 10, 5,  1};
    SplitMix64 external{RunSeed{7}};
    auto reference = external;
    for (const bool along_top : {false, true})
    {
        const RunSeed seed{along_top ? 2ULL : 0ULL};
        GameState expected{.player_position = *NodeId::from_value(0)};
        expected.sector_entry = *NodeId::from_value(0);
        expected.sector_exit = *NodeId::from_value(48);
        expected.receiver_nodes[along_top ? 4 : 28] = true;
        expected.receiver_nodes[along_top ? 20 : 44] = true;
        for (int i = 0; i < 13; ++i)
        {
            const int n = along_top ? (i <= 6 ? i : 6 + (i - 6) * 7)
                                    : (i <= 6 ? i * 7 : 42 + i - 6);
            expected.ports[n] =
                *PortMask::from_bits((along_top ? top : bottom)[i]);
            expected.rotatable_nodes[n] = i != 0 && i != 4 && i != 8 && i != 12;
        }
        for (int repeat = 0; repeat < 3; ++repeat)
        {
            const auto state = sector_fixture(seed);
            if (!test::same_state(state, expected) ||
                external.next_u64() != reference.next_u64())
            {
                std::cerr << "Full baseline state mismatch\n";
                return 1;
            }
            auto changed = state;
            changed.pulses.push_back(
                {*EntityId::from_value(0), *NodeId::from_value(0)});
            changed.ports[0] = PortMask{};
            changed.reserve = 0;
            if (!test::same_state(state, expected) ||
                test::same_state(changed, expected))
                return 1;
        }
    }
    return 0;
}

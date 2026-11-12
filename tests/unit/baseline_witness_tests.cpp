#include "../support/state_checks.hpp"
#include "corpus/baseline_witness.hpp"
#include <iostream>
using namespace closed_circuit;
using namespace closed_circuit::developer;
int main()
{
    for (const auto seed : {0ULL, 1ULL, 2ULL, UINT64_MAX})
    {
        const auto initial = sector_fixture(RunSeed{seed});
        const auto before = initial;
        const auto witness = build_baseline_witness(RunSeed{seed}, initial);
        if (!witness || witness->size() != (seed == 0 ? 18 : 17) ||
            !validate_solution(initial, *witness) ||
            !test::same_state(initial, before) ||
            build_baseline_witness(RunSeed{seed}, initial) != witness)
            return 1;
        auto state = initial;
        for (const auto &command : *witness)
            state = resolve_turn(state, command).state;
        auto expected = initial;
        expected.player_position = *NodeId::from_value(48);
        expected.turns_taken =
            TurnIndex{static_cast<std::uint32_t>(witness->size())};
        expected.reserve = 3;
        expected.powered_nodes = initial.receiver_nodes;
        const bool top = initial.receiver_nodes[4];
        for (int i = 1; i < 12; ++i)
        {
            const int n = top ? (i <= 6 ? i : 6 + (i - 6) * 7)
                              : (i <= 6 ? i * 7 : 42 + i - 6);
            expected.ports[n] = *PortMask::from_bits(
                i == 6 ? (top ? 12 : 3)
                       : (top ? (i < 6 ? 10 : 5) : (i < 6 ? 5 : 10)));
        }
        if (!test::same_state(state, expected))
        {
            std::cerr << "Full witness end state mismatch\n";
            return 1;
        }
        auto cut = *witness;
        cut.pop_back();
        if (validate_solution(initial, cut))
            return 1;
        for (int change = 0; change < 5; ++change)
        {
            auto unsupported = initial;
            if (change == 0)
                unsupported.overload_debt = 3;
            if (change == 1)
                unsupported.player_integrity = 1;
            if (change == 2)
                unsupported.blocked_nodes[48] = true;
            if (change == 3)
                unsupported.ports[24] = *PortMask::from_bits(15);
            if (change == 4)
                unsupported.pulses.push_back(
                    {*EntityId::from_value(0), *NodeId::from_value(0)});
            const auto copy = unsupported;
            if (build_baseline_witness(RunSeed{seed}, unsupported) ||
                !test::same_state(unsupported, copy))
            {
                std::cerr << "Unsupported baseline accepted\n";
                return 1;
            }
        }
    }
    return 0;
}

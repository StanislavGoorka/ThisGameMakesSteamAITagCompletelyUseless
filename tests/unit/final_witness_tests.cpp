#include "../support/state_checks.hpp"
#include "corpus/final_witness.hpp"

#include <iostream>

using namespace closed_circuit;
using namespace closed_circuit::developer;

int main()
{
    std::size_t maximum = 0;
    for (std::uint64_t index = 0; index <= 256; ++index)
    {
        const RunSeed seed{index == 256 ? UINT64_MAX : index};
        const auto initial = *build_sector_state(seed, SectorVariant::final);
        const auto before = initial;
        const auto witness = build_final_witness(seed, initial);
        if (!witness || witness->empty() || witness->size() > 48 ||
            !validate_solution(initial, *witness) ||
            build_final_witness(seed, initial) != witness ||
            !test::same_state(initial, before))
        {
            std::cerr << "Final certificate failed for corpus index " << index
                      << '\n';
            return 1;
        }
        maximum = std::max(maximum, witness->size());
        const auto commands_before = *witness;
        auto state = initial;
        for (const auto &command : *witness)
        {
            auto result = resolve_turn(state, command);
            if (!result.accepted)
                return 1;
            state = std::move(result.state);
        }
        auto expected = initial;
        expected.player_position = *initial.sector_exit;
        expected.powered_nodes = initial.receiver_nodes;
        expected.turns_taken =
            TurnIndex{static_cast<std::uint32_t>(witness->size())};
        expected.reserve = 3;
        const bool top = initial.receiver_nodes[4];
        for (int i = 1; i < 12; ++i)
        {
            const int n = top ? (i <= 6 ? i : 6 + (i - 6) * 7)
                              : (i <= 6 ? i * 7 : 42 + i - 6);
            expected.ports[n] = *PortMask::from_bits(
                i == 6 ? (top ? 12 : 3)
                       : (top ? (i < 6 ? 10 : 5) : (i < 6 ? 5 : 10)));
        }
        if (!test::same_state(state, expected) || *witness != commands_before ||
            std::ranges::count(state.powered_nodes, true) != 3 ||
            sector_outcome(state) != SectorOutcome::completed)
            return 1;
        auto bad = *witness;
        bad.pop_back();
        if (validate_solution(initial, bad))
            return 1;
        bad = *witness;
        bad.front() = MoveCommand{*NodeId::from_value(48)};
        if (validate_solution(initial, bad))
            return 1;
        bad = *witness;
        bad.push_back(WaitCommand{});
        if (validate_solution(initial, bad) ||
            validate_solution(initial, std::vector<Command>(48, WaitCommand{})))
            return 1;
        for (int field = 0; field < 5; ++field)
        {
            auto unsupported = initial;
            if (field == 0)
                unsupported.delivery_requirement = DeliveryRequirement{};
            if (field == 1)
                unsupported.player_integrity = 0;
            if (field == 2)
                unsupported.blocked_nodes[48] = true;
            if (field == 3)
                unsupported.terminal_rotation_used = true;
            if (field == 4)
                unsupported.overload_debt = 3;
            const auto copy = unsupported;
            if (build_final_witness(seed, unsupported) ||
                !test::same_state(copy, unsupported))
                return 1;
        }
        state.powered_nodes.fill(false);
        if (!test::same_state(initial, before))
            return 1;
    }
    std::cout << "Final certificates: 257; maximum accepted actions: "
              << maximum << '\n';
    return 0;
}

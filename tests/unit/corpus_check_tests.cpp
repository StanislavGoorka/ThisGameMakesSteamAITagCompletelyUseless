#include "../support/state_checks.hpp"
#include "corpus/corpus_check.hpp"
#include <iostream>
using namespace closed_circuit;
using namespace closed_circuit::developer;
int main()
{
    for (int mode = 0; mode < 3; ++mode)
    {
        auto state = sector_fixture(RunSeed{0});
        if (mode == 1)
            state.pulses = {{*EntityId::from_value(0), *state.sector_entry},
                            {*EntityId::from_value(1), *state.sector_entry}};
        if (mode == 2)
            state.player_integrity = 1;
        const auto before = state;
        const auto result = check_sector_seed(RunSeed{0}, state);
        const unsigned expected = mode == 0 ? 0 : mode == 1 ? 5 : 7;
        if (result.baseline_reason != expected ||
            !test::same_state(before, state))
        {
            std::cerr << "Corpus reason or full input mismatch\n";
            return 1;
        }
        if (mode == 0 && (result.turns != 18 ||
                          result.debt_reasons != std::array<unsigned, 7>{}))
            return 1;
    }
    return 0;
}

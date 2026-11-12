#include "corpus/corpus_check.hpp"
#include "corpus/fixed_seeds.hpp"
#include <iostream>
using namespace closed_circuit;
using namespace closed_circuit::developer;
int main(int argc, char **)
{
    if (argc != 1)
        return 1;
    std::size_t rejected_baselines = 0, rejected_scenarios = 0;
    std::cout << "corpus 1\nseed baseline turns debt0 debt2 debt3 debt6 "
                 "no_entry blocked occupied\n";
    for (const auto seed : sector_seeds)
    {
        const auto check =
            check_sector_seed(RunSeed{seed}, sector_fixture(RunSeed{seed}));
        rejected_baselines += check.baseline_reason != 0;
        std::cout << seed << ' ' << check.baseline_reason << ' ' << check.turns;
        for (const auto reason : check.debt_reasons)
        {
            rejected_scenarios += reason != 0;
            std::cout << ' ' << reason;
        }
        std::cout << '\n';
    }
    std::cout << "summary " << sector_seeds.size() << ' ' << rejected_baselines
              << ' ' << rejected_scenarios << '\n';
    return rejected_baselines || rejected_scenarios ? 1 : 0;
}

#include "corpus/fixed_seeds.hpp"
#include "strategy/wait_then_walk.hpp"

#include <charconv>
#include <iostream>
#include <sstream>
#include <string_view>
#include <vector>

using namespace closed_circuit;
using namespace closed_circuit::developer;

namespace
{
std::string_view end_name(StrategyEnd end)
{
    switch (end)
    {
    case StrategyEnd::won:
        return "won";
    case StrategyEnd::death:
        return "death";
    case StrategyEnd::overload:
        return "overload";
    case StrategyEnd::rejected:
        break;
    }
    return "rejected";
}
} // namespace

// developer_strategy SEED... | --corpus
// Every argument is parsed before any run; invalid input prints nothing.
int main(int argc, char **argv)
{
    std::vector<std::uint64_t> seeds;
    if (argc == 2 && std::string_view(argv[1]) == "--corpus")
        seeds.assign(sector_seeds.begin(), sector_seeds.end());
    else
        for (int index = 1; index < argc; ++index)
        {
            const std::string_view text = argv[index];
            std::uint64_t seed = 0;
            const auto [end, error] =
                std::from_chars(text.data(), text.data() + text.size(), seed);
            if (text.empty() || error != std::errc{} ||
                end != text.data() + text.size())
                return 1;
            seeds.push_back(seed);
        }
    if (seeds.empty())
        return 1;
    std::ostringstream report;
    report << "strategy 1 wait_then_walk\nseed sectors end first_turn\n";
    std::size_t first_sectors = 0, won = 0, rejected = 0;
    for (const auto seed : seeds)
    {
        const auto run = play_wait_then_walk(RunSeed{seed});
        first_sectors += run.sectors_completed > 0;
        won += run.end == StrategyEnd::won;
        rejected += run.end == StrategyEnd::rejected;
        report << seed << ' ' << run.sectors_completed << ' '
               << end_name(run.end) << ' ' << run.first_sector_turn << '\n';
    }
    report << "summary " << seeds.size() << ' ' << first_sectors << ' ' << won
           << '\n';
    std::cout << report.str();
    return rejected == 0 ? 0 : 2;
}

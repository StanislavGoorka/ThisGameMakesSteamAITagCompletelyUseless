#include "core/debt_spawn.hpp"
#include "core/turn_preview.hpp"
#include "corpus/fixed_seeds.hpp"
#include "corpus/sector_fixture.hpp"
#include "replay/state_hash.hpp"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>
using namespace closed_circuit;

// Keep the complete resolver call between the two clock reads in optimized
// builds.
#if defined(_MSC_VER)
__declspec(noinline)
#else
__attribute__((noinline))
#endif
    TurnResult measured_preview(const GameState &state, const Command &command)
{
    return preview_turn(state, command);
}

int main(int argc, char **)
{
    if (argc != 1)
        return 1;
    using Clock = std::chrono::steady_clock;
    std::uint64_t digest = 0;
    std::cout << "preview_benchmark 1\nthreads "
              << std::thread::hardware_concurrency() << '\n';
    for (unsigned family = 0; family < 3; ++family)
    {
        std::vector<GameState> corpus;
        for (const auto seed : developer::sector_seeds)
        {
            auto state = developer::sector_fixture(RunSeed{seed});
            if (family == 1)
            {
                state.overload_debt = 3;
                if (!spawn_debt_pulse(state))
                    return 2;
            }
            if (family == 2)
                for (unsigned node = 0; node < state.ports.size(); ++node)
                    state.pulses.push_back({*EntityId::from_value(node),
                                            *NodeId::from_value(node)});
            corpus.push_back(state);
        }
        for (unsigned rejected = 0; rejected < 2; ++rejected)
        {
            const Command command =
                rejected ? Command{MoveCommand{*NodeId::from_value(48)}}
                         : Command{WaitCommand{}};
            // Warm up and check semantics outside the measured samples.
            for (const auto &state : corpus)
            {
                const auto preview = measured_preview(state, command);
                const auto committed = resolve_turn(state, command);
                if (preview.accepted != (rejected == 0) ||
                    preview.accepted != committed.accepted ||
                    developer::state_hash(preview.state) !=
                        developer::state_hash(committed.state) ||
                    (!preview.accepted &&
                     developer::state_hash(preview.state) !=
                         developer::state_hash(state)))
                    return 2;
            }
            std::vector<std::int64_t> samples;
            for (unsigned repeat = 0; repeat < 3; ++repeat)
                for (const auto &state : corpus)
                {
                    const auto begin = Clock::now();
                    const auto result = measured_preview(state, command);
                    const auto end = Clock::now();
                    samples.push_back(
                        std::chrono::duration_cast<std::chrono::nanoseconds>(
                            end - begin)
                            .count());
                    // Hashing, result destruction and output are outside
                    // timing.
                    digest += developer::state_hash(result.state);
                }
            std::sort(samples.begin(), samples.end());
            std::cout << "case " << family << ' ' << rejected << ' '
                      << samples.size() << ' '
                      << samples[(samples.size() * 50 + 99) / 100 - 1] << ' '
                      << samples[(samples.size() * 95 + 99) / 100 - 1] << '\n';
        }
    }
    std::cout << "digest " << digest << '\n';
    return 0;
}

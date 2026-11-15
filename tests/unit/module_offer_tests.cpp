#include "../support/run_checks.hpp"
#include "application/checkpoint_decode.hpp"
#include "application/checkpoint_encode.hpp"
#include "application/reward_selection.hpp"
#include "application/run_checkpoint.hpp"
#include "core/module_offer.hpp"
#include "core/run_reward.hpp"
#include "core/run_start.hpp"
#include "core/run_transition.hpp"
#include "core/splitmix64.hpp"
#include <iostream>
#include <vector>
using namespace closed_circuit;
int main()
{
    std::uint64_t digest = 14695981039346656037ULL;
    for (std::size_t count = 0; count <= 12; ++count)
    {
        std::array<bool, 12> seen{};
        for (std::uint64_t seed = 0; seed < 257; ++seed)
            for (std::uint8_t sector = 1; sector <= 6; ++sector)
            {
                const auto indices =
                    module_offer_indices(count, RunSeed{seed}, sector);
                if (indices !=
                        module_offer_indices(count, RunSeed{seed}, sector) ||
                    indices[0].has_value() != (count > 0) ||
                    indices[1].has_value() != (count > 1))
                    return 2;
                if (indices[1] && *indices[1] <= *indices[0])
                    return 3;
                for (const auto index : indices)
                {
                    digest =
                        (digest ^ (index ? *index : 255)) * 1099511628211ULL;
                    if (index)
                    {
                        if (*index >= count)
                            return 4;
                        seen[*index] = true;
                    }
                }
            }
        for (std::size_t i = 0; i < count; ++i)
            if (!seen[i])
                return 5;
    }
    // Literal corpus digest from tools/corpus/module_offer_vectors.cjs.
    if (digest != 12087177321262304545ULL)
        return 8;
    if (module_offer_indices(3, RunSeed{0}, 1) != ModuleOfferIndices{0, 2} ||
        module_offer_indices(12, RunSeed{42}, 2) != ModuleOfferIndices{3, 9} ||
        module_offer_indices(12, RunSeed{UINT64_MAX}, 6) !=
            ModuleOfferIndices{2, 6})
        return 9;
    if (module_offer_indices(13, RunSeed{0}, 1) != ModuleOfferIndices{} ||
        module_offer_indices(SIZE_MAX, RunSeed{0}, 1) != ModuleOfferIndices{})
        return 6;
    // Default-context literals (seed 0, sector 1) for owned {}, {1}, {2} and
    // {1,2} from tools/corpus/module_offer_vectors.cjs offers.
    const auto id = [](unsigned raw)
    { return std::optional{static_cast<ModuleId>(raw)}; };
    const std::array<ModuleOffer, 4> expected{
        ModuleOffer{id(1), id(3)}, ModuleOffer{id(11), id(12)},
        ModuleOffer{id(11), id(12)}, ModuleOffer{}};
    SplitMix64 external{RunSeed{42}}, reference = external;
    for (const bool reverse : {false, true})
        for (unsigned mask = 0; mask < 4; ++mask)
        {
            ModuleLoadout modules;
            for (const unsigned bit : {reverse ? 2U : 1U, reverse ? 1U : 2U})
                if ((mask & bit) &&
                    !modules.add(bit == 1 ? ModuleId::rotation_range
                                          : ModuleId::terminal_rotation))
                    return 1;
            const auto before = modules;
            for (int repeat = 0; repeat < 3; ++repeat)
            {
                auto offer = module_offer(modules);
                if (offer != expected[mask] || modules != before ||
                    external.next_u64() != reference.next_u64())
                {
                    std::cerr << "Canonical unowned module offer mismatch\n";
                    return 1;
                }
                for (const auto slot : offer)
                    if (slot && modules.contains(*slot))
                        return 1;
                offer = {};
                if (module_offer(modules) != expected[mask] ||
                    modules != before)
                    return 1;
            }
        }
    // Admitted pool 1..12: every unowned ID is reachable, offers are two
    // distinct ascending unowned IDs unless the loadout is full. Literal
    // digest from tools/corpus/module_offer_vectors.cjs offers.
    std::uint64_t offers_digest = 14695981039346656037ULL;
    const std::array<std::vector<unsigned>, 6> loadouts{
        {{}, {1}, {2}, {1, 2}, {7}, {12}}};
    for (const auto &owned : loadouts)
    {
        ModuleLoadout modules;
        for (const auto raw : owned)
            if (!modules.add(static_cast<ModuleId>(raw)))
                return 10;
        const bool full = owned.size() == ModuleLoadout::slot_limit;
        std::array<bool, 13> seen{};
        for (std::uint64_t seed = 0; seed < 257; ++seed)
            for (std::uint8_t sector = 1; sector <= 6; ++sector)
            {
                const auto offer = module_offer(modules, RunSeed{seed}, sector);
                if (offer != module_offer(modules, RunSeed{seed}, sector) ||
                    offer[0].has_value() == full ||
                    offer[1].has_value() == full ||
                    (!full && *offer[0] >= *offer[1]))
                    return 11;
                for (const auto slot : offer)
                {
                    const unsigned raw =
                        slot ? static_cast<unsigned>(*slot) : 0U;
                    offers_digest = (offers_digest ^ raw) * 1099511628211ULL;
                    if (!slot)
                        continue;
                    if (raw < 1 || raw > 12 || modules.contains(*slot) ||
                        std::ranges::find(reward_module_catalog, *slot) ==
                            reward_module_catalog.end())
                        return 12;
                    seen[raw] = true;
                }
            }
        for (unsigned raw = 1; raw <= 12; ++raw)
            if (seen[raw] ==
                (full || modules.contains(static_cast<ModuleId>(raw))))
                return 13;
    }
    if (offers_digest != 18309029188938229860ULL)
        return 14;
    // Every sector start restored from its checkpoint bytes leads to the same
    // reward offer as the live run, which equals the run-context offer.
    for (const std::uint64_t raw : {0ULL, 1ULL, 42ULL, UINT64_MAX})
    {
        auto run = start_run(RunSeed{raw});
        for (unsigned number = 1; number < sector_count; ++number)
        {
            const auto checkpoint = capture_checkpoint(run);
            const auto bytes =
                checkpoint ? encode_checkpoint(*checkpoint) : std::nullopt;
            const auto decoded =
                bytes ? decode_checkpoint(*bytes) : std::nullopt;
            auto restored =
                decoded ? restore_checkpoint(*decoded) : std::nullopt;
            if (!restored || !test::same_run(*restored, run))
                return 15;
            for (auto *state : {&run, &*restored})
            {
                state->sector.powered_nodes = state->sector.receiver_nodes;
                state->sector.player_position = *state->sector.sector_exit;
            }
            const auto offer = reward_offer(run);
            if (offer != reward_offer(*restored) ||
                offer != module_offer(run.sector.modules, run.seed,
                                      static_cast<std::uint8_t>(number)) ||
                offer[0].has_value() !=
                    (run.sector.modules.size() < ModuleLoadout::slot_limit))
                return 16;
            if (offer[0] && !choose_run_module(run, *offer[0]))
                return 17;
            const auto next = advance_run(run);
            if (!next.accepted)
                return 17;
            run = next.state;
        }
    }
    return 0;
}

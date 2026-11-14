#include "core/module_offer.hpp"
#include "core/splitmix64.hpp"
#include <iostream>
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
    const std::array<ModuleOffer, 4> expected{
        ModuleOffer{ModuleId::rotation_range, ModuleId::terminal_rotation},
        ModuleOffer{ModuleId::terminal_rotation, std::nullopt},
        ModuleOffer{ModuleId::rotation_range, std::nullopt}, ModuleOffer{}};
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
                for (std::uint64_t seed = 0; seed < 257; ++seed)
                    for (std::uint8_t sector = 1; sector <= 6; ++sector)
                        if (module_offer(modules, RunSeed{seed}, sector) !=
                            expected[mask])
                            return 7;
                for (const auto slot : offer)
                    if (slot && modules.contains(*slot))
                        return 1;
                offer = {};
                if (module_offer(modules) != expected[mask] ||
                    modules != before)
                    return 1;
            }
        }
    return 0;
}

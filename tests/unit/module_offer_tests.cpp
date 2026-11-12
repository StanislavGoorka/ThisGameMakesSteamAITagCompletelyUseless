#include "core/module_offer.hpp"
#include "core/splitmix64.hpp"
#include <iostream>
using namespace closed_circuit;
int main()
{
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

#pragma once

#include "core/module_loadout.hpp"
#include "core/module_offer_indices.hpp"

namespace closed_circuit
{
using ModuleOffer = std::array<std::optional<ModuleId>, 2>;

// Offers contain only admitted, uninstalled modules in canonical order.
[[nodiscard]] constexpr ModuleOffer
module_offer(const ModuleLoadout &modules, RunSeed seed = RunSeed{0},
             std::uint8_t completed_sector = 1) noexcept
{
    ModuleOffer offer{};
    if (modules.size() == ModuleLoadout::slot_limit)
        return offer;
    std::size_t count = 0;
    std::array<ModuleId, reward_module_catalog.size()> pool{};
    for (const auto id : reward_module_catalog)
        if (!modules.contains(id))
            pool[count++] = id;
    const auto indices = module_offer_indices(count, seed, completed_sector);
    for (std::size_t i = 0; i < indices.size(); ++i)
        if (indices[i])
            offer[i] = pool[*indices[i]];
    return offer;
}
} // namespace closed_circuit

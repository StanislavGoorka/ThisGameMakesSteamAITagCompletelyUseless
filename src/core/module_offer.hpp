#pragma once

#include "core/module_loadout.hpp"

namespace closed_circuit
{
using ModuleOffer = std::array<std::optional<ModuleId>, 2>;

// Offers contain only admitted, uninstalled modules in canonical order.
[[nodiscard]] constexpr ModuleOffer
module_offer(const ModuleLoadout &modules) noexcept
{
    ModuleOffer offer{};
    if (modules.size() == ModuleLoadout::slot_limit)
        return offer;
    std::size_t count = 0;
    for (const auto id : reward_module_catalog)
        if (!modules.contains(id))
            offer[count++] = id;
    return offer;
}
} // namespace closed_circuit

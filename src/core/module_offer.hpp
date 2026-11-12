#pragma once

#include "core/module_loadout.hpp"

namespace closed_circuit
{
using ModuleOffer = std::array<std::optional<ModuleId>, 2>;

// The MVP catalog has two modules. Offers are compact, canonical and contain
// only uninstalled modules. Exhaustion produces an empty offer, never a repeat.
[[nodiscard]] constexpr ModuleOffer
module_offer(const ModuleLoadout &modules) noexcept
{
    ModuleOffer offer{};
    std::size_t count = 0;
    for (const auto id :
         {ModuleId::rotation_range, ModuleId::terminal_rotation})
        if (!modules.contains(id))
            offer[count++] = id;
    return offer;
}
} // namespace closed_circuit

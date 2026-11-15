#pragma once

#include <algorithm>
#include <array>
#include <concepts>
#include <cstdint>
#include <optional>
#include <utility>

namespace closed_circuit
{
// Stable internal IDs of start configurations, never player-facing labels.
// Zero is not a configuration. START_CONFIGURATION_PLAN reserves 3.
enum class StartConfigId : std::uint8_t
{
    baseline = 1,
    preinstalled_range = 2
};

// Only implemented configurations; a reserved ID is unknown until added here.
inline constexpr std::array start_config_catalog{
    StartConfigId::baseline, StartConfigId::preinstalled_range};

[[nodiscard]] constexpr bool is_known_start_config(StartConfigId id) noexcept
{
    return std::ranges::find(start_config_catalog, id) !=
           start_config_catalog.end();
}

template <std::integral Integer>
    requires(!std::same_as<Integer, bool>)
[[nodiscard]] constexpr std::optional<StartConfigId>
start_config_from_value(Integer value) noexcept
{
    if (!std::in_range<std::uint8_t>(value))
        return std::nullopt;
    const auto id = static_cast<StartConfigId>(value);
    if (!is_known_start_config(id))
        return std::nullopt;
    return id;
}
} // namespace closed_circuit

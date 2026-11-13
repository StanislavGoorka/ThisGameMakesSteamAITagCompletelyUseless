#pragma once
#include "application/asset_reference.hpp"
#include "core/port_mask.hpp"

namespace closed_circuit::presentation
{
inline constexpr unsigned atlas_width = 832, atlas_height = 373;
inline constexpr unsigned tile_size = 16, tile_step = 17;

struct AtlasTile
{
    unsigned column, row, quarter_turns;
    bool operator==(const AtlasTile &) const = default;
};

[[nodiscard]] constexpr bool valid_tile(AtlasTile tile) noexcept
{
    return tile.column < 49 && tile.row < 22 && tile.quarter_turns < 4;
}

// Pin the coordinates to the exact approved version, not merely an asset ID.
[[nodiscard]] inline const AssetReference &atlas_reference()
{
    static const auto reference = *AssetReference::from_fields(
        "kenney1bit12-monochrome",
        "assets/approved/kenney-1bit-1.2/Tilesheet/monochrome-transparent.png",
        "888B96CEF0777CE03E9A9F3EB2C7AFC02B2EED9C8F4E9E36E9871F8B624D2D00",
        19451);
    return reference;
}

// Whole existing sprites only. No port strokes or generated geometry.
[[nodiscard]] constexpr AtlasTile tile_for_ports(PortMask ports) noexcept
{
    if (ports.bits() == 0)
        return {9, 3, 0};
    constexpr unsigned bases[]{5, 6, 7, 15, 4};
    for (unsigned shape = 0; shape < 5; ++shape)
    {
        auto mask = *PortMask::from_bits(bases[shape]);
        for (unsigned turns = 0; turns < 4; ++turns)
        {
            if (mask.bits() == ports.bits())
                return {8 + shape, 0, turns};
            mask = mask.rotate_clockwise();
        }
    }
    return {9, 3, 0}; // All valid PortMask values are covered above.
}
} // namespace closed_circuit::presentation

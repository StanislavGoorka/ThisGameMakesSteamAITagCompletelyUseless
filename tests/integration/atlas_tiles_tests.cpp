#include "platform/windows/asset_manifest_parse.hpp"
#include "platform/windows/png_read.hpp"
#include "presentation/atlas_tiles.hpp"
#include "presentation/unit_projection.hpp"
#include <iostream>
using namespace closed_circuit;
namespace p = presentation;

int wmain(int argc, wchar_t **argv)
{
    if (argc != 2)
        return 1;
    winrt::init_apartment();
    const auto decoded = windows::read_png_asset(argv[1], p::atlas_reference());
    if (!decoded.image)
        return 2;
    const auto &image = *decoded.image;
    if (image.width != p::atlas_width || image.height != p::atlas_height)
        return 3;
    // Independent literal expectations: [column,row,clockwise quarters].
    constexpr p::AtlasTile expected[]{
        {9, 3, 0}, {12, 0, 2}, {12, 0, 3}, {9, 0, 3}, {12, 0, 0}, {8, 0, 0},
        {9, 0, 0}, {10, 0, 0}, {12, 0, 1}, {9, 0, 2}, {8, 0, 1},  {10, 0, 3},
        {9, 0, 1}, {10, 0, 2}, {10, 0, 1}, {11, 0, 0}};
    for (unsigned bits = 0; bits < 16; ++bits)
    {
        const auto tile = p::tile_for_ports(*PortMask::from_bits(bits));
        if (tile != expected[bits] || !p::valid_tile(tile))
            return 4;
        // Count alpha at the four original edges. Rotate the edge index,
        // independently of the bit-rotation helper used by production code.
        unsigned observed = 0;
        for (unsigned side = 0; side < 4; ++side)
        {
            unsigned count = 0;
            for (unsigned n = 0; n < 16; ++n)
            {
                const auto x = tile.column * 17 + (side == 1   ? 15
                                                   : side == 3 ? 0
                                                               : n);
                const auto y = tile.row * 17 + (side == 0   ? 0
                                                : side == 2 ? 15
                                                            : n);
                count += image.pixels[y * image.stride + x * 4 + 3] != 0;
            }
            if (count)
            {
                if (count != 12)
                    return 5;
                observed |= 1u << ((side + tile.quarter_turns) % 4);
            }
        }
        if (observed != bits)
        {
            std::cerr << "Port mismatch for mask " << bits << '\n';
            return 6;
        }
    }
    if (p::valid_tile({49, 0, 0}) || p::valid_tile({0, 22, 0}) ||
        p::valid_tile({0, 0, 4}) || !p::valid_tile({48, 21, 3}) ||
        PortMask::from_bits(16))
        return 7;
    for (unsigned kind = 0; kind < 8; ++kind)
    {
        const auto tile = *p::tile_for_symbol(static_cast<p::SymbolKind>(kind));
        unsigned alpha = 0;
        for (unsigned y = 0; y < 16; ++y)
            for (unsigned x = 0; x < 16; ++x)
                alpha += image.pixels[(tile.row * 17 + y) * image.stride +
                                      (tile.column * 17 + x) * 4 + 3];
        if (!p::valid_tile(tile) || !alpha)
            return 8;
    }
    return 0;
}

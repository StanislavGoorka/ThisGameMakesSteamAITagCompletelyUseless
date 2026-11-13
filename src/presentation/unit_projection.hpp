#pragma once
#include "presentation/board_projection.hpp"

namespace closed_circuit::presentation
{
enum class SymbolKind : std::uint8_t
{
    player,
    packet,
    pulse,
    entry,
    exit,
    receiver,
    powered_receiver,
    fixed
};
[[nodiscard]] constexpr std::optional<AtlasTile>
tile_for_symbol(SymbolKind kind) noexcept
{
    switch (kind)
    {
    case SymbolKind::player:
        return AtlasTile{24, 0, 0};
    case SymbolKind::packet:
        return AtlasTile{45, 8, 0};
    case SymbolKind::pulse:
        return AtlasTile{25, 7, 0};
    case SymbolKind::entry:
        return AtlasTile{19, 20, 0};
    case SymbolKind::exit:
        return AtlasTile{20, 20, 0};
    case SymbolKind::receiver:
        return AtlasTile{39, 14, 0};
    case SymbolKind::powered_receiver:
        return AtlasTile{39, 15, 0};
    case SymbolKind::fixed:
        return AtlasTile{38, 13, 0};
    }
    return std::nullopt;
}
struct SymbolDraw
{
    SymbolKind kind;
    NodeId node;
    std::optional<EntityId> entity;
    TileDraw draw;
    bool operator==(const SymbolDraw &) const = default;
};

[[nodiscard]] inline std::optional<std::vector<SymbolDraw>>
unit_draws(const GameState &state, float width, float height)
{
    const auto layout = fit_board(width, height);
    if (!layout)
        return std::nullopt;
    std::vector<SymbolDraw> result;
    const auto append = [&](SymbolKind kind, NodeId node, unsigned corner,
                            std::optional<EntityId> entity = std::nullopt)
    {
        const auto x = layout->left + (node.value() % 7) * layout->cell;
        const auto y = layout->top + (node.value() / 7) * layout->cell;
        result.push_back(
            {kind,
             node,
             entity,
             {*tile_for_symbol(kind), x + (corner % 2) * .75f * layout->cell,
              y + (corner / 2) * .75f * layout->cell, layout->cell * .25f}});
    };
    // One field-role symbol: receivers (including power) precede exit/entry,
    // then fixed nodes. These roles never hide a unit's separate corner.
    for (unsigned i = 0; i < GameState::node_count; ++i)
    {
        const auto node = *NodeId::from_value(i);
        std::optional<SymbolKind> kind;
        if (state.receiver_nodes[i])
            kind = state.powered_nodes[i] ? SymbolKind::powered_receiver
                                          : SymbolKind::receiver;
        else if (state.sector_exit == node)
            kind = SymbolKind::exit;
        else if (state.sector_entry == node)
            kind = SymbolKind::entry;
        else if (state.ports[i].bits() && !state.rotatable_nodes[i])
            kind = SymbolKind::fixed;
        if (kind)
            append(*kind, node, 0);
    }
    if (state.player_alive())
        append(SymbolKind::player, state.player_position, 2);
    if (state.packet)
        append(SymbolKind::packet, state.packet->position, 3);
    auto pulses = state.pulses;
    std::ranges::sort(pulses, {}, &Pulse::id);
    for (const auto &pulse : pulses)
        append(SymbolKind::pulse, pulse.position, 1, pulse.id);
    return result;
}
} // namespace closed_circuit::presentation

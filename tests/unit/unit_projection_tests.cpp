#include "../support/state_checks.hpp"
#include "core/turn_resolver.hpp"
#include "presentation/unit_projection.hpp"
using namespace closed_circuit;
namespace p = presentation;
int main()
{
    for (unsigned position = 0; position < 49; ++position)
    {
        const auto node = *NodeId::from_value(position);
        GameState state{.player_position = node};
        state.packet = Packet{node, *NodeId::from_value(48)};
        state.pulses.push_back({*EntityId::from_value(9), node});
        const auto original = state;
        const auto draws = p::unit_draws(state, 420, 280);
        if (!draws || draws->size() != 3)
            return 1;
        const float x = 70.f + (position % 7) * 40;
        const float y = (position / 7) * 40.f;
        constexpr p::SymbolKind kinds[]{
            p::SymbolKind::player, p::SymbolKind::packet, p::SymbolKind::pulse};
        constexpr unsigned columns[]{24, 45, 25}, rows[]{0, 8, 7};
        for (unsigned i = 0; i < 3; ++i)
        {
            const auto &symbol = (*draws)[i];
            if (symbol.node != node || symbol.kind != kinds[i] ||
                symbol.draw.tile != p::AtlasTile{columns[i], rows[i], 0} ||
                symbol.draw.size != 10 || symbol.draw.x != x + (i ? 30 : 0) ||
                symbol.draw.y != y + (i == 2 ? 0 : 30) ||
                symbol.entity != (i == 2
                                      ? std::optional{*EntityId::from_value(9)}
                                      : std::nullopt))
                return 2;
        }
        if (!test::same_state(state, original))
            return 3;
        state.packet.reset();
        state.pulses.clear();
        state.player_integrity = 0;
        if (!p::unit_draws(state, 112, 112)->empty())
            return 4;
    }
    GameState state{.player_position = *NodeId::from_value(0)};
    state.player_integrity = 0;
    state.sector_entry = *NodeId::from_value(0);
    state.sector_exit = *NodeId::from_value(48);
    state.receiver_nodes[1] = true;
    state.receiver_nodes[2] = true;
    state.powered_nodes[2] = true;
    state.ports[3] = *PortMask::from_bits(1);
    state.pulses = {{*EntityId::from_value(9), *NodeId::from_value(9)},
                    {*EntityId::from_value(1), *NodeId::from_value(8)}};
    const auto original = state;
    const auto symbols = *p::unit_draws(state, 112, 112);
    constexpr p::SymbolKind roles[]{p::SymbolKind::entry,
                                    p::SymbolKind::receiver,
                                    p::SymbolKind::powered_receiver,
                                    p::SymbolKind::fixed, p::SymbolKind::exit};
    if (symbols.size() != 7)
        return 5;
    for (unsigned i = 0; i < 5; ++i)
        if (symbols[i].kind != roles[i] || !p::valid_tile(symbols[i].draw.tile))
            return 6;
    if (symbols[5].entity->value() != 1 || symbols[6].entity->value() != 9 ||
        !test::same_state(state, original) || p::unit_draws(state, 0, 112) ||
        p::tile_for_symbol(static_cast<p::SymbolKind>(255)))
        return 7;
    // Actual delivery and pulse contact remove both transient symbols.
    GameState before{.player_position = *NodeId::from_value(0)};
    before.ports[0] = *PortMask::from_bits(2);
    before.ports[1] = *PortMask::from_bits(8);
    before.rotatable_nodes[0] = before.rotatable_nodes[1] = true;
    before.receiver_nodes[1] = true;
    before.packet = Packet{*NodeId::from_value(0), *NodeId::from_value(1)};
    before.pulses = {{*EntityId::from_value(1), *NodeId::from_value(1)}};
    const auto after = resolve_turn(before, WaitCommand{});
    const auto projected = *p::unit_draws(after.state, 112, 112);
    if (!after.accepted || after.state.player_integrity != 2 ||
        projected.size() != 2 ||
        projected[0].kind != p::SymbolKind::powered_receiver ||
        projected[1].kind != p::SymbolKind::player)
        return 8;
    return 0;
}

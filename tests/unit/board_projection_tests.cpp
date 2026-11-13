#include "../support/state_checks.hpp"
#include "presentation/board_projection.hpp"
#include <limits>
using namespace closed_circuit;
namespace p = presentation;

int main()
{
    GameState state{.player_position = *NodeId::from_value(24)};
    for (unsigned i = 0; i < 49; ++i)
        state.ports[i] = *PortMask::from_bits(i % 16);
    const auto original = state;
    struct View
    {
        float w, h, left, top, cell;
    };
    constexpr View views[]{{112, 112, 0, 0, 16},
                           {420, 280, 70, 0, 40},
                           {280, 420, 0, 70, 40},
                           {7, 7, 0, 0, 1},
                           {1, 1, 0, 0, 1.f / 7}};
    for (const auto view : views)
    {
        const auto draws = p::board_draws(state, view.w, view.h);
        if (!draws || draws->size() != 49 ||
            p::fit_board(view.w, view.h) !=
                p::BoardLayout{view.left, view.top, view.cell})
            return 1;
        for (unsigned row = 0; row < 7; ++row)
            for (unsigned col = 0; col < 7; ++col)
            {
                const auto &draw = (*draws)[row * 7 + col];
                if (draw.x != view.left + col * view.cell ||
                    draw.y != view.top + row * view.cell ||
                    draw.size != view.cell ||
                    draw.tile != p::tile_for_ports(state.ports[row * 7 + col]))
                    return 2;
                const auto expected = NodeId::from_value(row * 7 + col);
                const auto right = view.left + (col + 1) * view.cell;
                const auto bottom = view.top + (row + 1) * view.cell;
                for (const auto x : {draw.x, draw.x + draw.size / 2,
                                     std::nextafter(right, draw.x)})
                    for (const auto y : {draw.y, draw.y + draw.size / 2,
                                         std::nextafter(bottom, draw.y)})
                        if (p::board_node_at(x, y, view.w, view.h) != expected)
                            return 5;
            }
        const auto right = view.left + 7 * view.cell;
        const auto bottom = view.top + 7 * view.cell;
        if (p::board_node_at(right, view.top, view.w, view.h) ||
            p::board_node_at(view.left, bottom, view.w, view.h) ||
            p::board_node_at(std::nextafter(view.left, -1.f), view.top, view.w,
                             view.h) ||
            p::board_node_at(view.left, std::nextafter(view.top, -1.f), view.w,
                             view.h))
            return 6;
    }
    const auto nan = std::numeric_limits<float>::quiet_NaN();
    const auto inf = std::numeric_limits<float>::infinity();
    for (const auto invalid : {0.f, -1.f, nan, inf, 1000001.f})
        if (p::board_draws(state, invalid, 112) ||
            p::board_draws(state, 112, invalid) ||
            p::board_node_at(0, 0, invalid, 112) ||
            p::board_node_at(0, 0, 112, invalid))
            return 3;
    for (const auto invalid : {nan, inf, -inf})
        if (p::board_node_at(invalid, 0, 112, 112) ||
            p::board_node_at(0, invalid, 112, 112))
            return 7;
    if (p::board_node_at(100, 100, 420, 280) != NodeId::from_value(14) ||
        p::board_node_at(100, 100, 280, 420) != NodeId::from_value(2))
        return 8;
    auto copy = *p::board_draws(state, 112, 112);
    copy[0].x = 99;
    copy[0].tile = {48, 21, 3};
    if ((*p::board_draws(state, 112, 112))[0].x != 0 ||
        !test::same_state(state, original))
        return 4;
    return 0;
}

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
            }
    }
    const auto nan = std::numeric_limits<float>::quiet_NaN();
    const auto inf = std::numeric_limits<float>::infinity();
    for (const auto invalid : {0.f, -1.f, nan, inf, 1000001.f})
        if (p::board_draws(state, invalid, 112) ||
            p::board_draws(state, 112, invalid))
            return 3;
    auto copy = *p::board_draws(state, 112, 112);
    copy[0].x = 99;
    copy[0].tile = {48, 21, 3};
    if ((*p::board_draws(state, 112, 112))[0].x != 0 ||
        !test::same_state(state, original))
        return 4;
    return 0;
}

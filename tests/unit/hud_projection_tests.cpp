#include "../support/state_checks.hpp"
#include "presentation/hud_projection.hpp"
#include <limits>
using namespace closed_circuit;

int main()
{
    GameState state{.player_position = *NodeId::from_value(0)};
    // A powered transit node is not a delivery.
    state.powered_nodes.fill(true);
    for (std::size_t count = 0; count <= GameState::node_count; ++count)
    {
        if (count)
            state.receiver_nodes[count - 1] = true;
        for (auto hp : {0, 1, 3, 255})
            for (auto turn :
                 {0u, 47u, 48u, 49u, std::numeric_limits<std::uint32_t>::max()})
            {
                state.player_integrity = static_cast<std::uint8_t>(hp);
                state.turns_taken = TurnIndex{turn};
                const auto before = state;
                const auto values = presentation::hud_values(state);
                if (values.integrity != hp ||
                    static_cast<std::uint32_t>(values.turns) != turn ||
                    values.deliveries != count ||
                    !test::same_state(state, before))
                    return 1;
            }
    }
    state.powered_nodes.fill(false);
    return presentation::hud_values(state).deliveries == 0 ? 0 : 2;
}

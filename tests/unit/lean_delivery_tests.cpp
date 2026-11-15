#include "../support/run_checks.hpp"
#include "application/checkpoint_decode.hpp"
#include "application/checkpoint_encode.hpp"
#include "application/run_checkpoint.hpp"
#include "application/session_flow.hpp"
#include "core/packet_delivery.hpp"
#include "core/turn_preview.hpp"

#include <algorithm>
#include <cstdint>
#include <iostream>

using namespace closed_circuit;

namespace
{
bool check(bool condition, const char *message)
{
    if (!condition)
        std::cerr << message << '\n';
    return condition;
}

NodeId node(int value)
{
    return *NodeId::from_value(value);
}

// Only 24-25-26 are connected; 26 is a receiver and the player has no ports.
GameState corridor(int packet, int mask, std::uint8_t reserve)
{
    GameState state{.player_position = node(32),
                    .packet = Packet{node(packet), node(26)},
                    .sector_entry = node(24),
                    .sector_exit = node(48)};
    state.ports[24] = *PortMask::from_bits(PortMask::east_bit);
    state.ports[25] =
        *PortMask::from_bits(PortMask::east_bit | PortMask::west_bit);
    state.ports[26] = *PortMask::from_bits(PortMask::west_bit);
    state.receiver_nodes[26] = state.receiver_nodes[40] = true;
    state.reserve = reserve;
    state.overload_debt = 5;
    // An unparsed mask falls back to none, so the expectations fail plainly.
    state.difficulty =
        DifficultyModifiers::from_value(mask).value_or(DifficultyModifiers{});
    return state;
}

int expected_reserve(int reserve, bool module, bool lean)
{
    if (reserve >= GameState::reserve_limit)
        return reserve;
    return std::min<int>(GameState::reserve_limit,
                         reserve + (module ? 2 : 1) - (lean ? 1 : 0));
}

// Every mask, reserve and delivery module: only the restored amount differs.
bool delivery_step()
{
    for (const int mask : {0, 1, 2, 3})
        for (int modules = 0; modules < 4; ++modules)
            for (std::uint8_t reserve = 0; reserve <= 3; ++reserve)
            {
                auto state = corridor(26, mask, reserve);
                if (modules & 1)
                    static_cast<void>(
                        state.modules.add(ModuleId::delivery_reserve));
                if (modules & 2)
                    static_cast<void>(
                        state.modules.add(ModuleId::delivery_debt_relief));
                auto expected = state;
                expected.reserve = static_cast<std::uint8_t>(expected_reserve(
                    reserve, (modules & 1) != 0, (mask & 2) != 0));
                expected.overload_debt = modules & 2 ? 4 : 5;
                expected.powered_nodes[26] = true;
                expected.packet.reset();
                if (!deliver_packet(state) ||
                    !test::same_state(state, expected))
                {
                    std::cerr << "mask=" << mask << " modules=" << modules
                              << " reserve=" << int{reserve} << '\n';
                    return false;
                }
                // A repeated step delivers nothing and changes nothing.
                if (!check(!deliver_packet(state) &&
                               test::same_state(state, expected),
                           "A second delivery step must change nothing"))
                    return false;
            }
    return true;
}

// The resolver and its preview share the rule across a whole turn.
bool delivery_turn()
{
    for (const int mask : {0, 2})
    {
        const auto state = corridor(25, mask, 1);
        const auto result = resolve_turn(state, WaitCommand{});
        const auto preview = preview_turn(state, WaitCommand{});
        const std::uint8_t reserve = mask == 2 ? 1 : 2;
        if (!check(result.accepted && !result.state.packet &&
                       result.state.powered_nodes[26] &&
                       result.state.reserve == reserve &&
                       result.state.turns_taken == TurnIndex{1} &&
                       preview.accepted &&
                       test::same_state(preview.state, result.state),
                   "A delivering turn must restore one reserve less"))
            return false;
    }
    return true;
}

bool persistence()
{
    for (const int mask : {2, 3})
        for (const auto seed : {RunSeed{0}, RunSeed{1}, RunSeed{UINT64_MAX}})
            for (const auto config : start_config_catalog)
            {
                const auto parsed = DifficultyModifiers::from_value(mask);
                if (!check(parsed.has_value(), "Mask must be implemented"))
                    return false;
                const auto modifiers = *parsed;
                const auto run = *start_run(seed, config, modifiers);
                const auto checkpoint = capture_checkpoint(run);
                const auto bytes =
                    checkpoint ? encode_checkpoint(*checkpoint) : std::nullopt;
                if (!check(bytes && (*bytes)[31] == mask &&
                               decode_checkpoint(*bytes) == checkpoint,
                           "lean_delivery must round-trip through byte 31"))
                    return false;
                auto restored = *restore_checkpoint(*checkpoint);
                restored.sector.player_integrity = 0;
                SessionFlow resumed{true};
                if (!check(resumed.confirm(restored) &&
                               test::same_run(restored, run) &&
                               restored.sector.difficulty == modifiers,
                           "Restart after resume must keep lean_delivery"))
                    return false;
            }
    return true;
}
} // namespace

int main()
{
    return delivery_step() && delivery_turn() && persistence() ? 0 : 1;
}

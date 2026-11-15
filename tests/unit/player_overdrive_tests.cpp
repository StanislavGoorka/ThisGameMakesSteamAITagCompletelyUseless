#include "application/checkpoint_decode.hpp"
#include "application/checkpoint_encode.hpp"
#include "core/player_overdrive.hpp"
#include "core/run_transition.hpp"
#include "core/turn_preview.hpp"
#include <algorithm>
#include <cstdlib>
#include <iostream>
using namespace closed_circuit;
namespace
{
NodeId node(int n)
{
    return *NodeId::from_value(n);
}
bool same_state(const GameState &left, const GameState &right)
{
    if (left.difficulty != right.difficulty ||
        left.first_overdrive_used != right.first_overdrive_used ||
        left.delivery_requirement != right.delivery_requirement)
        return false;
    return left.terminal_rotation_used == right.terminal_rotation_used &&
           left.modules == right.modules && left.reserve == right.reserve &&
           left.overload_debt == right.overload_debt &&
           left.player_position == right.player_position &&
           left.player_integrity == right.player_integrity &&
           left.turns_taken == right.turns_taken &&
           left.packet == right.packet &&
           left.powered_nodes == right.powered_nodes &&
           left.receiver_nodes == right.receiver_nodes &&
           left.sector_entry == right.sector_entry &&
           left.sector_exit == right.sector_exit &&
           left.pulses == right.pulses &&
           left.rotatable_nodes == right.rotatable_nodes &&
           left.blocked_nodes == right.blocked_nodes &&
           std::ranges::equal(left.ports, right.ports, {}, &PortMask::bits,
                              &PortMask::bits);
}

GameState board(int source)
{
    GameState s{.player_position = node(source)};
    s.rotatable_nodes.fill(true);
    s.blocked_nodes.fill(true);
    s.packet = Packet{node(0), node(48)};
    s.sector_entry = node(0);
    s.sector_exit = node(48);
    s.receiver_nodes[48] = true;
    s.powered_nodes[1] = true;
    s.pulses = {{*EntityId::from_value(7), node(24)}};
    s.turns_taken = TurnIndex{48};
    s.player_integrity = 0;
    return s;
}
} // namespace
int main()
{
    for (const bool installed : {false, true})
        for (const bool used : {false, true})
            for (const std::uint32_t debt :
                 {0U, 1U, UINT32_MAX - 1, UINT32_MAX})
                for (const unsigned reserve : {0U, 1U, 3U})
                    for (const bool invalid : {false, true})
                        for (const unsigned partner : {0U, 1U, 2U, 3U})
                        {
                            GameState state{.player_position = node(24)};
                            state.ports[25] = *PortMask::from_bits(1);
                            state.rotatable_nodes[25] = true;
                            state.reserve = static_cast<std::uint8_t>(reserve);
                            state.overload_debt = debt;
                            state.first_overdrive_used = used;
                            if (installed &&
                                !state.modules.add(
                                    ModuleId::first_overdrive_debt))
                                return 2;
                            if (partner && !state.modules.add(
                                               static_cast<ModuleId>(partner)))
                                return 2;
                            const auto before = state;
                            auto expected = state;
                            const bool charge = !installed || used;
                            const bool accepted =
                                !invalid && reserve > 0 &&
                                (!charge || debt < UINT32_MAX);
                            if (accepted)
                            {
                                expected.ports[25] = *PortMask::from_bits(4);
                                --expected.reserve;
                                if (charge)
                                    ++expected.overload_debt;
                                expected.first_overdrive_used = true;
                            }
                            const auto target = node(invalid ? 48 : 25);
                            if (overdrive_from_player(state, target) !=
                                    accepted ||
                                !same_state(state, expected))
                                return 3;
                            auto turn_expected = expected;
                            if (accepted)
                                turn_expected.turns_taken = TurnIndex{1};
                            const Command command = OverdriveCommand{target};
                            for (unsigned repeat = 0; repeat < 3; ++repeat)
                            {
                                const auto preview =
                                    preview_turn(before, command);
                                const auto commit =
                                    resolve_turn(before, command);
                                if (preview.accepted != accepted ||
                                    commit.accepted != accepted ||
                                    !same_state(preview.state, turn_expected) ||
                                    !same_state(commit.state, turn_expected))
                                    return 4;
                            }
                            if (accepted)
                            {
                                auto second = expected;
                                const bool second_ok =
                                    second.reserve > 0 &&
                                    second.overload_debt < UINT32_MAX;
                                if (second_ok)
                                {
                                    second.ports[25] = *PortMask::from_bits(1);
                                    --second.reserve;
                                    ++second.overload_debt;
                                }
                                if (overdrive_from_player(state, node(25)) !=
                                        second_ok ||
                                    !same_state(state, second))
                                    return 5;
                            }
                        }
    ModuleLoadout first_debt;
    if (!first_debt.add(ModuleId::first_overdrive_debt))
        return 6;
    for (unsigned number = 1; number <= sector_count; ++number)
    {
        const RunCheckpoint cp{
            RunSeed{7}, *SectorNumber::from_value(number), 3, 2, 0, first_debt,
            false};
        const auto bytes = encode_checkpoint(cp);
        auto restored = restore_checkpoint(cp);
        if (!bytes || decode_checkpoint(*bytes) != cp || !restored ||
            restored->sector.first_overdrive_used)
            return 7;
        if (number < sector_count)
        {
            restored->sector.first_overdrive_used = true;
            restored->sector.powered_nodes = restored->sector.receiver_nodes;
            restored->sector.player_position = *restored->sector.sector_exit;
            restored->reward_claimed = true;
            const auto next = advance_run(*restored);
            if (!next.accepted || next.state.sector.first_overdrive_used ||
                !capture_checkpoint(next.state))
                return 8;
        }
    }
    // Exhaust every source/target pair, including row wrapping, diagonals and
    // edges.
    for (int source = 0; source < 49; ++source)
        for (int target = 0; target < 49; ++target)
        {
            auto s = board(source);
            s.overload_debt = 19;
            const int distance = std::abs(source / 7 - target / 7) +
                                 std::abs(source % 7 - target % 7);
            auto expected = s;
            if (distance <= 1)
            {
                --expected.reserve;
                ++expected.overload_debt;
                expected.first_overdrive_used = true;
            }
            if (overdrive_from_player(s, node(target)) != (distance <= 1) ||
                !same_state(s, expected))
            {
                std::cerr << "Range/atomic state mismatch\n";
                return 1;
            }
        }
    for (unsigned mask = 0; mask < 16; ++mask)
        for (int reserve : {0, 1, 3})
            for (std::uint32_t debt : {0U, UINT32_MAX - 1, UINT32_MAX})
                for (bool rotatable : {false, true})
                {
                    auto s = board(24);
                    s.ports[25] = *PortMask::from_bits(mask);
                    s.reserve = static_cast<std::uint8_t>(reserve);
                    s.overload_debt = debt;
                    s.rotatable_nodes[25] = rotatable;
                    const bool accepted =
                        reserve > 0 && debt < UINT32_MAX && rotatable;
                    auto expected = s;
                    if (accepted)
                    {
                        expected.ports[25] = *PortMask::from_bits(
                            ((mask << 2) | (mask >> 2)) & 15);
                        --expected.reserve;
                        ++expected.overload_debt;
                        expected.first_overdrive_used = true;
                    }
                    if (overdrive_from_player(s, node(25)) != accepted ||
                        !same_state(s, expected))
                    {
                        std::cerr
                            << "Mask/resources/immutable target mismatch\n";
                        return 1;
                    }
                }
    return 0;
}

#include "../support/run_checks.hpp"
#include "application/checkpoint_decode.hpp"
#include "application/checkpoint_encode.hpp"
#include "application/run_checkpoint.hpp"
#include "core/turn_preview.hpp"

using namespace closed_circuit;
namespace
{
NodeId node(unsigned n)
{
    return *NodeId::from_value(n);
}
bool add(GameState &state, std::optional<ModuleId> id)
{
    return !id || state.modules.add(*id);
}
bool same_turns(const GameState &state, const Command &command,
                const GameState &expected)
{
    const auto before = state;
    for (unsigned repeat = 0; repeat < 3; ++repeat)
    {
        const auto preview = preview_turn(state, command);
        const auto result = resolve_turn(state, command);
        const auto rejected = resolve_turn(state, MoveCommand{node(0)});
        if (!preview.accepted || !result.accepted || rejected.accepted ||
            !test::same_state(preview.state, expected) ||
            !test::same_state(result.state, expected) ||
            !test::same_state(rejected.state, before) ||
            !test::same_state(state, before))
            return false;
    }
    return true;
}
} // namespace
int main()
{
    constexpr std::array partners{std::optional<ModuleId>{},
                                  std::optional{ModuleId::packet_stride},
                                  std::optional{ModuleId::delivery_reserve}};
    constexpr std::array<unsigned, 5> candidates{3, 9, 17, 30, 46};
    // Every receiver/powered subset of five nodes; powered non-receivers at
    // both ends of the board never become targets.
    for (const bool installed : {false, true})
        for (const auto partner : partners)
            for (unsigned receivers = 0; receivers < 32; ++receivers)
                for (unsigned powered = 0; powered < 32; ++powered)
                {
                    GameState state{.player_position = node(24)};
                    state.powered_nodes[0] = state.powered_nodes[48] = true;
                    std::optional<NodeId> expected;
                    for (unsigned bit = 0; bit < 5; ++bit)
                    {
                        const auto index = candidates[bit];
                        state.receiver_nodes[index] = (receivers >> bit) & 1U;
                        state.powered_nodes[index] = (powered >> bit) & 1U;
                        const bool open = ((receivers & ~powered) >> bit) & 1U;
                        if (open && (installed || !expected))
                            expected = node(index);
                    }
                    if (!add(state,
                             installed
                                 ? std::optional{ModuleId::packet_priority}
                                 : std::nullopt) ||
                        !add(state, partner))
                        return 1;
                    const auto before = state;
                    if (next_packet_target(state) != expected ||
                        !test::same_state(state, before))
                        return 2;
                    // Introduction uses the same target from the entry; with
                    // no route the packet waits there. An active packet with
                    // the opposite target keeps it.
                    state.sector_entry = node(20);
                    auto introduced = state;
                    introduced.turns_taken = TurnIndex{1};
                    if (expected)
                        introduced.packet = Packet{node(20), *expected};
                    if (!same_turns(state, WaitCommand{}, introduced))
                        return 3;
                    auto active = state;
                    active.packet = Packet{node(20), node(40)};
                    auto kept = active;
                    kept.turns_taken = TurnIndex{1};
                    if (!same_turns(active, WaitCommand{}, kept))
                        return 4;
                }
    // Line 0..6 with receivers 2, 4 and 6: an independent simulation of
    // introduction, movement (one or two edges) and single delivery per turn.
    for (const bool installed : {false, true})
        for (const bool stride : {false, true})
        {
            GameState state{.player_position = node(48)};
            state.ports[0] = *PortMask::from_bits(2);
            for (unsigned i = 1; i < 6; ++i)
                state.ports[i] = *PortMask::from_bits(10);
            state.ports[6] = *PortMask::from_bits(8);
            state.sector_entry = node(0);
            state.receiver_nodes[2] = state.receiver_nodes[4] =
                state.receiver_nodes[6] = true;
            state.reserve = 0;
            if (!add(state, installed ? std::optional{ModuleId::packet_priority}
                                      : std::nullopt) ||
                !add(state, stride ? std::optional{ModuleId::packet_stride}
                                   : std::nullopt))
                return 5;
            std::optional<std::pair<unsigned, unsigned>> packet;
            std::array<bool, 7> lit{};
            unsigned delivered = 0;
            for (unsigned turn = 1; turn <= 14; ++turn)
            {
                if (!packet)
                    for (unsigned k = 0; k < 3; ++k)
                    {
                        const unsigned target =
                            installed ? 6 - 2 * k : 2 + 2 * k;
                        if (!lit[target])
                        {
                            packet = std::pair{0U, target};
                            break;
                        }
                    }
                if (packet)
                {
                    packet->first = std::min(packet->second,
                                             packet->first + (stride ? 2 : 1));
                    if (packet->first == packet->second)
                    {
                        lit[packet->second] = true;
                        packet.reset();
                        ++delivered;
                    }
                }
                const auto next = resolve_turn(state, WaitCommand{});
                state = next.state;
                const bool match =
                    next.accepted && state.turns_taken == TurnIndex{turn} &&
                    state.packet.has_value() == packet.has_value() &&
                    (!packet ||
                     (state.packet->position == node(packet->first) &&
                      state.packet->target == node(packet->second))) &&
                    state.powered_nodes[2] == lit[2] &&
                    state.powered_nodes[4] == lit[4] &&
                    state.powered_nodes[6] == lit[6] &&
                    state.reserve == std::min(delivered, 3U);
                if (!match)
                    return 6;
            }
            if (delivered != 3)
                return 7;
        }
    // Restored normal and final sectors: the first introduced packet aims at
    // the highest receiver; the codec keeps the slot in both pairings.
    for (const unsigned number : {1U, 2U, 6U})
        for (const auto partner : {std::optional<ModuleId>{},
                                   std::optional{ModuleId::packet_stride}})
            for (const bool installed : {false, true})
            {
                ModuleLoadout modules;
                if ((installed && !modules.add(ModuleId::packet_priority)) ||
                    (partner && !modules.add(*partner)))
                    return 8;
                const RunCheckpoint checkpoint{
                    RunSeed{5}, *SectorNumber::from_value(number),
                    3,          2,
                    0,          modules,
                    false};
                const auto bytes = encode_checkpoint(checkpoint);
                const auto decoded =
                    bytes ? decode_checkpoint(*bytes) : std::nullopt;
                const auto restored =
                    decoded ? restore_checkpoint(*decoded) : std::nullopt;
                if (decoded != checkpoint || !restored ||
                    capture_checkpoint(*restored) != checkpoint)
                    return 9;
                std::vector<unsigned> receivers;
                for (unsigned index = 0; index < GameState::node_count; ++index)
                    if (restored->sector.receiver_nodes[index])
                        receivers.push_back(index);
                if (receivers.size() != (number == sector_count ? 3U : 2U))
                    return 10;
                const auto turn = resolve_turn(restored->sector, WaitCommand{});
                const auto aim =
                    installed ? receivers.back() : receivers.front();
                if (!turn.accepted || !turn.state.packet ||
                    turn.state.packet->target != node(aim))
                    return 11;
            }
    return 0;
}

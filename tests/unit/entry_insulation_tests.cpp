#include "../support/run_checks.hpp"
#include "application/checkpoint_decode.hpp"
#include "application/checkpoint_encode.hpp"
#include "application/run_checkpoint.hpp"
#include "core/run_start.hpp"
#include "core/run_transition.hpp"
#include "core/turn_preview.hpp"

using namespace closed_circuit;
namespace
{
NodeId node(unsigned n)
{
    return *NodeId::from_value(n);
}
EntityId id(unsigned n)
{
    return *EntityId::from_value(n);
}
// Line 0-1-2 with the entry at node 0.
GameState line(unsigned player, bool has_entry)
{
    GameState state{.player_position = node(player)};
    state.ports[0] = *PortMask::from_bits(2);
    state.ports[1] = *PortMask::from_bits(10);
    state.ports[2] = *PortMask::from_bits(8);
    if (has_entry)
        state.sector_entry = node(0);
    state.overload_debt = 5;
    return state;
}
bool same_turns(const GameState &state, const Command &command,
                const GameState &expected)
{
    const auto before = state;
    for (unsigned repeat = 0; repeat < 3; ++repeat)
    {
        const auto preview = preview_turn(state, command);
        const auto result = resolve_turn(state, command);
        const auto rejected = resolve_turn(state, MoveCommand{node(48)});
        if (!preview.accepted || !result.accepted || rejected.accepted ||
            !test::same_state(preview.state, expected) ||
            !test::same_state(result.state, expected) ||
            !test::same_state(rejected.state, before) ||
            !test::same_state(state, before))
            return false;
    }
    return true;
}
RunState completed(std::uint64_t seed, unsigned number)
{
    auto run = start_run(RunSeed{seed});
    run.sector_number = *SectorNumber::from_value(number);
    run.sector.player_position = node(48);
    run.sector.powered_nodes = run.sector.receiver_nodes;
    run.sector.turns_taken = TurnIndex{47};
    run.reward_claimed = true;
    return run;
}
} // namespace
int main()
{
    constexpr std::array partners{
        std::optional<ModuleId>{}, std::optional{ModuleId::pulse_cadence},
        std::optional{ModuleId::powered_pulse_barrier}};
    for (const bool installed : {false, true})
        for (const auto partner : partners)
            for (const bool powered : {false, true})
                for (const bool has_entry : {false, true})
                    for (const unsigned turns : {0U, 1U})
                        for (const unsigned start : {0U, 1U})
                            for (const unsigned pulse_at : {0U, 1U, 2U})
                                for (const bool moves_player : {false, true})
                                {
                                    auto state = line(start, has_entry);
                                    state.turns_taken = TurnIndex{turns};
                                    state.receiver_nodes[1] =
                                        state.powered_nodes[1] = powered;
                                    state.pulses = {{id(4), node(pulse_at)}};
                                    if (installed &&
                                        !state.modules.add(
                                            ModuleId::entry_insulation))
                                        return 1;
                                    if (partner && !state.modules.add(*partner))
                                        return 1;
                                    // Independent line oracle.
                                    const unsigned after =
                                        moves_player ? 1 - start : start;
                                    auto expected = state;
                                    expected.player_position = node(after);
                                    expected.turns_taken = TurnIndex{turns + 1};
                                    bool contact = pulse_at == after;
                                    const bool pulses_move =
                                        partner != ModuleId::pulse_cadence ||
                                        (turns + 1) % 2 == 0;
                                    if (!contact && pulses_move)
                                    {
                                        const unsigned next =
                                            pulse_at < after ? pulse_at + 1
                                                             : pulse_at - 1;
                                        const bool barrier =
                                            partner ==
                                                ModuleId::
                                                    powered_pulse_barrier &&
                                            powered && next == 1;
                                        if (!barrier)
                                        {
                                            expected.pulses[0].position =
                                                node(next);
                                            contact = next == after;
                                        }
                                    }
                                    const bool shielded =
                                        installed && turns == 0 && has_entry &&
                                        after == 0;
                                    if (contact)
                                    {
                                        expected.pulses.clear();
                                        if (!shielded)
                                            --expected.player_integrity;
                                    }
                                    const Command command =
                                        moves_player
                                            ? Command{MoveCommand{node(after)}}
                                            : Command{WaitCommand{}};
                                    if (!same_turns(state, command, expected))
                                        return 2;
                                }
    // The helper removes the pulse without using the shared damage limit;
    // a later emitter or capacitor in the same turn still deals 1 HP.
    for (const bool installed : {false, true})
    {
        auto state = line(0, true);
        state.pulses = {{id(0), node(0)}};
        if (installed && !state.modules.add(ModuleId::entry_insulation))
            return 3;
        auto contact = state;
        bool used = false;
        if (!resolve_pulse_contact(contact, id(0), used) || used == installed ||
            !contact.pulses.empty() ||
            contact.player_integrity != (installed ? 3 : 2) ||
            contact.overload_debt != 5)
            return 3;
        for (const bool emitter : {false, true})
        {
            auto threatened = state;
            if (emitter)
                threatened.emitters = {
                    *PeriodicEmitter::from_values(id(7), node(2), 2, 1, 2)};
            else
                threatened.capacitors = {
                    *DelayedCapacitor::from_values(id(7), node(2), 1, 2)};
            auto expected = threatened;
            expected.pulses.clear();
            expected.player_integrity = 2;
            expected.turns_taken = TurnIndex{1};
            if (!emitter)
                static_cast<void>(expected.capacitors[0].advance_fuse());
            if (!same_turns(threatened, WaitCommand{}, expected))
                return 4;
        }
    }
    // A paid debt pulse waits on the entry after every boundary and restore;
    // insulation changes only integrity, never the debt already paid.
    for (const std::uint64_t seed : {0ULL, 1ULL, UINT64_MAX})
        for (const unsigned number : {1U, 4U, 5U})
            for (const auto partner :
                 {std::optional<ModuleId>{},
                  std::optional{ModuleId::terminal_rotation}})
            {
                std::array<std::optional<RunState>, 2> sectors;
                for (const bool installed : {false, true})
                {
                    auto run = completed(seed, number);
                    run.sector.overload_debt = 5;
                    if (installed &&
                        !run.sector.modules.add(ModuleId::entry_insulation))
                        return 5;
                    if (partner && !run.sector.modules.add(*partner))
                        return 5;
                    const auto advanced = advance_run(run);
                    const auto &sector = advanced.state.sector;
                    if (!advanced.accepted || sector.overload_debt != 2 ||
                        sector.pulses.size() != 1 ||
                        sector.pulses[0].position != *sector.sector_entry ||
                        sector.player_position != *sector.sector_entry ||
                        sector.turns_taken != TurnIndex{0})
                        return 5;
                    const auto checkpoint = capture_checkpoint(advanced.state);
                    const auto bytes = checkpoint
                                           ? encode_checkpoint(*checkpoint)
                                           : std::nullopt;
                    const auto decoded =
                        bytes ? decode_checkpoint(*bytes) : std::nullopt;
                    const auto restored =
                        decoded ? restore_checkpoint(*decoded) : std::nullopt;
                    if (!checkpoint || !checkpoint->entry_debt_pulse ||
                        decoded != checkpoint || !restored ||
                        !test::same_run(*restored, advanced.state))
                        return 6;
                    sectors[installed ? 1 : 0] = restored;
                }
                // Staying on the entry absorbs the contact; leaving it does
                // not, because the pulse then catches the player elsewhere.
                // Open both neighbours so that the entry can be left at once.
                for (auto &sector : sectors)
                    sector->sector.ports[1] = sector->sector.ports[7] =
                        *PortMask::from_bits(15);
                const auto &plain = sectors[0]->sector;
                const auto &shielded = sectors[1]->sector;
                std::optional<NodeId> away;
                for (const unsigned target : {1U, 7U})
                    if (is_command_legal(plain, MoveCommand{node(target)}))
                        away = node(target);
                if (!away)
                    return 7;
                for (const bool leave : {false, true})
                {
                    const Command command = leave ? Command{MoveCommand{*away}}
                                                  : Command{WaitCommand{}};
                    const auto base = resolve_turn(plain, command);
                    auto expected = base.state;
                    expected.modules = shielded.modules;
                    if (!leave)
                        ++expected.player_integrity;
                    if (!base.accepted || !base.state.pulses.empty() ||
                        base.state.player_integrity != 2 ||
                        base.state.overload_debt != 2 ||
                        !same_turns(shielded, command, expected))
                        return 8;
                    // The second accepted turn of the sector is unprotected.
                    auto second = shielded;
                    second.turns_taken = TurnIndex{1};
                    const auto later = resolve_turn(second, WaitCommand{});
                    if (!later.accepted || !later.state.pulses.empty() ||
                        later.state.player_integrity != 2)
                        return 9;
                }
            }
    return 0;
}

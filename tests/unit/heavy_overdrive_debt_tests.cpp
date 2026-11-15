#include "../support/run_checks.hpp"
#include "application/checkpoint_decode.hpp"
#include "application/checkpoint_encode.hpp"
#include "application/run_checkpoint.hpp"
#include "application/session_flow.hpp"
#include "core/command.hpp"
#include "core/run_transition.hpp"
#include "core/turn_preview.hpp"

#include <cstdint>
#include <iostream>
#include <limits>

using namespace closed_circuit;

namespace
{
constexpr auto debt_max = std::numeric_limits<std::uint32_t>::max();

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

DifficultyModifiers mask(int bits)
{
    // An unknown mask falls back to none, so the expectations fail plainly.
    return DifficultyModifiers::from_value(bits).value_or(
        DifficultyModifiers{});
}

// The player stands on a rotatable node; overdrive targets that node.
GameState board(int bits, std::uint32_t debt, bool waiver, bool used)
{
    GameState state{.player_position = node(24)};
    state.ports[24] = *PortMask::from_bits(PortMask::north_bit);
    state.rotatable_nodes[24] = true;
    state.reserve = 2;
    state.overload_debt = debt;
    state.first_overdrive_used = used;
    state.difficulty = mask(bits);
    if (waiver)
        static_cast<void>(state.modules.add(ModuleId::first_overdrive_debt));
    return state;
}

// Independent cost: the waiver frees the first overdrive, bit4 doubles.
bool overdrive_step()
{
    constexpr std::uint32_t debts[]{
        0, 5, debt_max - 3, debt_max - 2, debt_max - 1, debt_max};
    for (const int bits : {0, 1, 2, 3, 4, 5, 6, 7})
        for (const auto debt : debts)
            for (const bool waiver : {false, true})
                for (const bool used : {false, true})
                {
                    const auto before = board(bits, debt, waiver, used);
                    const std::uint32_t cost =
                        waiver && !used ? 0 : ((bits & 4) != 0 ? 2 : 1);
                    const bool legal = debt <= debt_max - cost;
                    auto state = before;
                    const OverdriveCommand command{node(24)};
                    auto expected = before;
                    if (legal)
                    {
                        expected.overload_debt = debt + cost;
                        expected.reserve = 1;
                        expected.first_overdrive_used = true;
                        expected.ports[24] =
                            *PortMask::from_bits(PortMask::south_bit);
                    }
                    if (overdrive_from_player(state, node(24)) != legal ||
                        !test::same_state(state, expected) ||
                        is_command_legal(before, command) != legal)
                    {
                        std::cerr << "bits=" << bits << " debt=" << debt
                                  << " waiver=" << waiver << " used=" << used
                                  << '\n';
                        return false;
                    }
                }
    return true;
}

// Resolver and preview charge the same doubled debt; reserve 0 still refuses.
bool overdrive_turn()
{
    for (const int bits : {0, 4})
    {
        const auto state = board(bits, 1, false, false);
        const auto result = resolve_turn(state, OverdriveCommand{node(24)});
        const auto preview = preview_turn(state, OverdriveCommand{node(24)});
        if (!check(result.accepted &&
                       result.state.overload_debt == (bits == 4 ? 3U : 2U) &&
                       preview.accepted &&
                       test::same_state(preview.state, result.state),
                   "A full turn must charge the modified debt"))
            return false;
        auto empty = state;
        empty.reserve = 0;
        const auto refused = resolve_turn(empty, OverdriveCommand{node(24)});
        if (!check(!refused.accepted && test::same_state(refused.state, empty),
                   "Overdrive without reserve must still refuse"))
            return false;
    }
    return true;
}

std::optional<NodeId> rotatable_neighbour(const GameState &state)
{
    const auto player = state.player_position.value();
    for (const int offset : {-7, 1, 7, -1})
    {
        const int candidate = player + offset;
        if (candidate < 0 || candidate >= 49 ||
            ((offset == 1 || offset == -1) && candidate / 7 != player / 7))
            continue;
        if (state.rotatable_nodes[candidate])
            return node(candidate);
    }
    return std::nullopt;
}

// Completes sector 1 like run_transition_tests: exit, powered, reward taken.
RunState completed(RunState run)
{
    run.sector.player_position = *run.sector.sector_exit;
    run.sector.powered_nodes = run.sector.receiver_nodes;
    run.reward_claimed = true;
    return run;
}

// ID3 starts with debt2: one doubled overdrive reaches debt4, so the next
// entry spawns one pulse and keeps the remainder1, which a checkpoint holds.
bool charged_debt_entry()
{
    for (const auto seed : {RunSeed{0}, RunSeed{1}, RunSeed{UINT64_MAX}})
        for (const int bits : {0, 4})
        {
            const auto run =
                *start_run(seed, StartConfigId::charged_debt, mask(bits));
            const auto target = rotatable_neighbour(run.sector);
            if (!check(target.has_value(), "Sector 1 needs a rotatable target"))
                return false;
            const auto after = resolve_run_turn(run, OverdriveCommand{*target});
            if (!check(after.accepted && after.state.sector.overload_debt ==
                                             (bits == 4 ? 4U : 3U),
                       "One overdrive must charge debt from 2"))
                return false;
            const auto next = advance_run(completed(after.state));
            const auto quiet = advance_run(completed(run));
            if (!check(next.accepted && next.state.sector.pulses.size() == 1 &&
                           next.state.sector.overload_debt ==
                               (bits == 4 ? 1U : 0U) &&
                           quiet.accepted &&
                           quiet.state.sector.pulses.empty() &&
                           quiet.state.sector.overload_debt == 2,
                       "The entry must spawn one pulse only after overdrive"))
                return false;
            const auto checkpoint = capture_checkpoint(next.state);
            const auto bytes =
                checkpoint ? encode_checkpoint(*checkpoint) : std::nullopt;
            const auto decoded =
                bytes ? decode_checkpoint(*bytes) : std::nullopt;
            const auto restored =
                decoded ? restore_checkpoint(*decoded) : std::nullopt;
            if (!check(restored && checkpoint->entry_debt_pulse &&
                           checkpoint->overload_debt == (bits == 4 ? 1U : 0U) &&
                           (*bytes)[31] == bits &&
                           test::same_run(*restored, next.state),
                       "The paid pulse boundary must round-trip"))
                return false;
        }
    return true;
}

bool persistence()
{
    for (const int bits : {4, 7})
        for (const auto seed : {RunSeed{0}, RunSeed{1}, RunSeed{UINT64_MAX}})
            for (const auto config : start_config_catalog)
            {
                const auto parsed = DifficultyModifiers::from_value(bits);
                if (!check(parsed.has_value(), "Mask must be implemented"))
                    return false;
                const auto run = *start_run(seed, config, *parsed);
                const auto checkpoint = capture_checkpoint(run);
                const auto bytes =
                    checkpoint ? encode_checkpoint(*checkpoint) : std::nullopt;
                if (!check(bytes && (*bytes)[31] == bits &&
                               decode_checkpoint(*bytes) == checkpoint,
                           "heavy_overdrive_debt must round-trip"))
                    return false;
                auto restored = *restore_checkpoint(*checkpoint);
                restored.sector.player_integrity = 0;
                SessionFlow resumed{true};
                if (!check(resumed.confirm(restored) &&
                               test::same_run(restored, run),
                           "Restart after resume must keep the mask"))
                    return false;
            }
    return true;
}
} // namespace

int main()
{
    return overdrive_step() && overdrive_turn() && charged_debt_entry() &&
                   persistence()
               ? 0
               : 1;
}

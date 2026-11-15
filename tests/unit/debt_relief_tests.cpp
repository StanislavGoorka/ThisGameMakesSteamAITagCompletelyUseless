#include "../support/run_checks.hpp"
#include "application/checkpoint_decode.hpp"
#include "application/checkpoint_encode.hpp"
#include "application/run_checkpoint.hpp"
#include "core/turn_preview.hpp"

#include <limits>

using namespace closed_circuit;
namespace
{
constexpr auto max_debt = std::numeric_limits<std::uint32_t>::max();
NodeId node(unsigned n)
{
    return *NodeId::from_value(n);
}
enum class Delivery
{
    none,
    duplicate,
    real
};
bool same_turns(const GameState &state, const Command &command,
                const std::optional<GameState> &expected)
{
    const auto before = state;
    for (unsigned repeat = 0; repeat < 3; ++repeat)
    {
        const auto preview = preview_turn(state, command);
        const auto result = resolve_turn(state, command);
        const auto rejected = resolve_turn(state, MoveCommand{node(48)});
        if (preview.accepted != expected.has_value() ||
            result.accepted != expected.has_value() || rejected.accepted ||
            !test::same_state(preview.state, expected.value_or(before)) ||
            !test::same_state(result.state, expected.value_or(before)) ||
            !test::same_state(rejected.state, before) ||
            !test::same_state(state, before))
            return false;
    }
    return true;
}
bool add(GameState &state, std::optional<ModuleId> id)
{
    return !id || state.modules.add(*id);
}
} // namespace
int main()
{
    constexpr std::array partners{std::optional<ModuleId>{},
                                  std::optional{ModuleId::delivery_reserve},
                                  std::optional{ModuleId::first_overdrive_debt},
                                  std::optional{ModuleId::packet_stride}};
    for (const bool installed : {false, true})
        for (const auto partner : partners)
            for (const std::uint32_t debt :
                 {0U, 1U, 2U, max_debt - 1, max_debt})
                for (const unsigned reserve : {0U, 1U, 2U, 3U})
                    for (const auto delivery :
                         {Delivery::none, Delivery::duplicate, Delivery::real})
                        for (const bool overdrive : {false, true})
                            for (const bool first_used : {false, true})
                            {
                                GameState state{.player_position = node(0)};
                                state.rotatable_nodes[1] = true;
                                state.ports[1] = *PortMask::from_bits(3);
                                state.reserve =
                                    static_cast<std::uint8_t>(reserve);
                                state.overload_debt = debt;
                                state.first_overdrive_used = first_used;
                                if (delivery != Delivery::none)
                                {
                                    state.receiver_nodes[20] = true;
                                    state.powered_nodes[20] =
                                        delivery == Delivery::duplicate;
                                    state.packet = Packet{node(20), node(20)};
                                }
                                if (!add(
                                        state,
                                        installed
                                            ? std::
                                                  optional{ModuleId::
                                                               delivery_debt_relief}
                                            : std::nullopt) ||
                                    !add(state, partner))
                                    return 1;
                                // Independent oracle of the turn order:
                                // validation, overdrive cost, then delivery.
                                const bool charge =
                                    partner != ModuleId::first_overdrive_debt ||
                                    first_used;
                                const bool legal =
                                    !overdrive ||
                                    (reserve > 0 &&
                                     !(charge && debt == max_debt));
                                std::optional<GameState> expected;
                                if (legal)
                                {
                                    auto next = state;
                                    next.turns_taken = TurnIndex{1};
                                    std::uint32_t owed = debt;
                                    unsigned left = reserve;
                                    if (overdrive)
                                    {
                                        next.ports[1] =
                                            *PortMask::from_bits(12);
                                        next.first_overdrive_used = true;
                                        --left;
                                        owed += charge ? 1U : 0U;
                                    }
                                    if (delivery == Delivery::real)
                                    {
                                        next.packet.reset();
                                        next.powered_nodes[20] = true;
                                        left = std::min(
                                            3U,
                                            left +
                                                (partner == ModuleId::
                                                                delivery_reserve
                                                     ? 2U
                                                     : 1U));
                                        if (installed && owed > 0)
                                            --owed;
                                    }
                                    next.reserve =
                                        static_cast<std::uint8_t>(left);
                                    next.overload_debt = owed;
                                    expected = next;
                                }
                                const Command command =
                                    overdrive
                                        ? Command{OverdriveCommand{node(1)}}
                                        : Command{WaitCommand{}};
                                if (!same_turns(state, command, expected))
                                    return 2;
                            }
    // Stride reaches the target in one turn; relief follows the single
    // delivery and is never repeated without another delivery.
    for (const bool installed : {false, true})
        for (const bool stride : {false, true})
        {
            GameState state{.player_position = node(0)};
            state.ports[22] = *PortMask::from_bits(2);
            state.ports[23] = *PortMask::from_bits(10);
            state.ports[24] = *PortMask::from_bits(8);
            state.receiver_nodes[24] = true;
            state.packet = Packet{node(22), node(24)};
            state.overload_debt = 2;
            if (!add(state, installed
                                ? std::optional{ModuleId::delivery_debt_relief}
                                : std::nullopt) ||
                !add(state, stride ? std::optional{ModuleId::packet_stride}
                                   : std::nullopt))
                return 3;
            const auto first = resolve_turn(state, WaitCommand{});
            const auto second = resolve_turn(first.state, WaitCommand{});
            const auto third = resolve_turn(second.state, WaitCommand{});
            const auto relieved = installed ? 1U : 2U;
            if (!first.accepted || !second.accepted || !third.accepted ||
                first.state.overload_debt != (stride ? relieved : 2U) ||
                first.state.packet.has_value() != !stride ||
                second.state.overload_debt != relieved || second.state.packet ||
                !second.state.powered_nodes[24] ||
                third.state.overload_debt != relieved ||
                third.state.reserve != 3)
                return 4;
        }
    // Restored boundaries keep the module: the first delivery after resume
    // relieves exactly one point, also next to a paid entry pulse.
    for (const auto partner :
         {std::optional<ModuleId>{}, std::optional{ModuleId::delivery_reserve},
          std::optional{ModuleId::first_overdrive_debt},
          std::optional{ModuleId::entry_insulation}})
        for (const std::uint32_t debt : {0U, 1U, 2U, max_debt - 3})
            for (const unsigned number : {1U, 3U, 6U})
            {
                ModuleLoadout modules;
                if (!modules.add(ModuleId::delivery_debt_relief) ||
                    (partner && !modules.add(*partner)))
                    return 5;
                const RunCheckpoint checkpoint{
                    RunSeed{0x0123456789abcdefULL},
                    *SectorNumber::from_value(number),
                    3,
                    1,
                    debt,
                    modules,
                    debt >= 3};
                const auto bytes = encode_checkpoint(checkpoint);
                const auto decoded =
                    bytes ? decode_checkpoint(*bytes) : std::nullopt;
                const auto restored =
                    decoded ? restore_checkpoint(*decoded) : std::nullopt;
                if (decoded != checkpoint || !restored ||
                    capture_checkpoint(*restored) != checkpoint)
                    return 6;
                auto sector = restored->sector;
                std::optional<NodeId> receiver;
                for (unsigned index = 0; index < GameState::node_count; ++index)
                    if (!receiver && sector.receiver_nodes[index])
                        receiver = node(index);
                if (!receiver)
                    return 7;
                sector.packet = Packet{*receiver, *receiver};
                auto plain = sector;
                plain.modules = {};
                if (!add(plain, partner) || !deliver_packet(sector) ||
                    !deliver_packet(plain))
                    return 8;
                auto expected = plain;
                expected.modules = modules;
                expected.overload_debt = debt > 0 ? debt - 1 : 0;
                if (!test::same_state(sector, expected) ||
                    plain.overload_debt != debt)
                    return 9;
            }
    return 0;
}

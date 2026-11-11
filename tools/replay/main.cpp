#include "replay/replay_runner.hpp"

#include <charconv>
#include <cstdint>
#include <iostream>
#include <optional>
#include <string_view>
#include <system_error>
#include <vector>

namespace
{

using namespace closed_circuit;

template <typename Integer>
std::optional<Integer> parse_decimal(std::string_view token)
{
    if (token.empty() || token.find_first_not_of("0123456789") != token.npos)
    {
        return std::nullopt;
    }
    Integer value{};
    const auto result =
        std::from_chars(token.data(), token.data() + token.size(), value, 10);
    if (result.ec != std::errc{} || result.ptr != token.data() + token.size())
    {
        return std::nullopt;
    }
    return value;
}

std::optional<Command> parse_command(std::string_view token)
{
    if (token == "wait")
    {
        return WaitCommand{};
    }
    const bool move = token.starts_with("move:");
    const bool rotate = token.starts_with("rotate:");
    if (!move && !rotate)
    {
        return std::nullopt;
    }
    const auto value = parse_decimal<std::uint64_t>(token.substr(move ? 5 : 7));
    const auto node = value ? NodeId::from_value(*value) : std::nullopt;
    if (!node)
    {
        return std::nullopt;
    }
    return move ? Command{MoveCommand{*node}} : Command{RotateCommand{*node}};
}

unsigned outcome_code(SectorOutcome outcome)
{
    switch (outcome)
    {
    case SectorOutcome::ongoing:
        return 0;
    case SectorOutcome::completed:
        return 1;
    case SectorOutcome::death:
        return 2;
    case SectorOutcome::overload:
        return 3;
    }
    return 3;
}

int invalid_arguments()
{
    std::cerr << "Invalid developer replay arguments or unsupported version\n";
    return 1;
}

} // namespace

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        return invalid_arguments();
    }
    const auto version = parse_decimal<std::uint32_t>(argv[1]);
    const auto seed = parse_decimal<std::uint64_t>(argv[2]);
    if (!version || !seed)
    {
        return invalid_arguments();
    }
    std::vector<closed_circuit::Command> commands;
    commands.reserve(static_cast<std::size_t>(argc - 3));
    for (int index = 3; index < argc; ++index)
    {
        const auto command = parse_command(argv[index]);
        if (!command)
        {
            return invalid_arguments();
        }
        commands.push_back(*command);
    }
    const auto replay = closed_circuit::developer::run_replay(
        *version, closed_circuit::RunSeed{*seed}, commands);
    if (!replay)
    {
        return invalid_arguments();
    }
    std::cout << "0 " << replay->initial_hash << '\n';
    std::size_t index = 0;
    for (const auto &step : replay->steps)
    {
        std::cout << ++index << ' ' << (step.accepted ? 1 : 0) << ' '
                  << outcome_code(step.outcome) << ' ' << step.hash << '\n';
    }
    return 0;
}

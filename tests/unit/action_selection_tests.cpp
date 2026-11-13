#include "../support/run_checks.hpp"
#include "application/action_selection.hpp"
#include "core/run_start.hpp"
#include <array>
using namespace closed_circuit;

int main()
{
    ActionSelection model;
    const auto run = start_run(RunSeed{7});
    const auto original = run;
    const auto player = *NodeId::from_value(24);
    const auto target = *NodeId::from_value(25);
    if (model.planning() || model.candidate() || model.pointed() ||
        model.confirm())
        return 1;
    model.point(target);
    if (model.choose(InputKey::rotate, player) !=
            Command{RotateCommand{target}} ||
        model.choose(InputKey::overdrive, player) !=
            Command{OverdriveCommand{target}})
        return 2;
    model.toggle();
    for (const auto key : std::array{
             InputKey::north, InputKey::east, InputKey::south, InputKey::west,
             InputKey::rotate, InputKey::overdrive, InputKey::wait})
    {
        const auto expected =
            key == InputKey::rotate      ? Command{RotateCommand{target}}
            : key == InputKey::overdrive ? Command{OverdriveCommand{target}}
                                         : *map_key_command(key, player);
        if (model.choose(key, player) || model.candidate() != expected ||
            model.confirm() != expected || model.confirm() || model.candidate())
            return 3;
    }
    if (model.choose(InputKey::wait, player) || !model.candidate())
        return 4;
    model.context_changed();
    if (model.candidate() || model.confirm())
        return 5;
    static_cast<void>(model.choose(InputKey::wait, player));
    model.cancel();
    if (model.planning() || model.candidate() || model.confirm())
        return 6;
    model.toggle();
    static_cast<void>(model.choose(InputKey::wait, player));
    model.toggle();
    model.toggle();
    if (model.candidate() || model.confirm())
        return 7;
    model.reset();
    if (model.planning() || model.candidate() || model.pointed() ||
        model.confirm())
        return 8;
    if (model.choose(InputKey::rotate, player) !=
            Command{RotateCommand{player}} ||
        model.choose(InputKey::overdrive, player) !=
            Command{OverdriveCommand{player}})
        return 9;
    for (int value = 0; value < 49; ++value)
        for (unsigned key = 0; key < 4; ++key)
        {
            const auto node = *NodeId::from_value(value);
            const auto input = static_cast<InputKey>(key);
            model.toggle();
            static_cast<void>(model.choose(InputKey::wait, node));
            if (model.choose(input, node) ||
                model.confirm() != map_key_command(input, node))
                return 10;
            model.cancel();
        }
    model.toggle();
    static_cast<void>(model.choose(InputKey::wait, player));
    static_cast<void>(model.choose(InputKey::count, player));
    if (model.candidate() || !test::same_run(run, original))
        return 11;
    return 0;
}

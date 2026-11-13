#include "application/keyboard_input.hpp"
using namespace closed_circuit;

int main()
{
    for (unsigned value = 0; value < static_cast<unsigned>(InputKey::count);
         ++value)
    {
        const auto key = static_cast<InputKey>(value);
        KeyboardInput input;
        if (input.key_down(key, false))
            return 1;
        input.set_focus(true);
        if (!input.key_down(key, false))
            return 2;
        // A redundant focus notification must not reset held keys.
        input.set_focus(true);
        for (int repeat = 0; repeat < 100; ++repeat)
            if (input.key_down(key, true) || input.key_down(key, false))
                return 3;
        input.key_up(key);
        if (!input.key_down(key, false))
            return 4;
        input.set_focus(false);
        if (input.key_down(key, false))
            return 5;
        input.set_focus(true);
        if (input.key_down(key, true))
            return 6;
        input.key_up(key);
        if (!input.key_down(key, false))
            return 7;
    }
    KeyboardInput input;
    input.set_focus(true);
    for (unsigned value = 0; value < static_cast<unsigned>(InputKey::count);
         ++value)
        if (!input.key_down(static_cast<InputKey>(value), false))
            return 8;
    for (const auto unknown : {InputKey::count, static_cast<InputKey>(999)})
    {
        input.key_up(unknown);
        if (input.key_down(unknown, false))
            return 9;
    }
    input.key_up(InputKey::north);
    if (!input.key_down(InputKey::north, false) ||
        input.key_down(InputKey::east, false))
        return 10;
    return 0;
}

#include "core/module_loadout.hpp"
#include <iostream>
using namespace closed_circuit;
static_assert(static_cast<unsigned>(ModuleId::rotation_range) == 1);
static_assert(static_cast<unsigned>(ModuleId::terminal_rotation) == 2);
static_assert(ModuleLoadout::slot_limit == 2);
// Independent test boundary; do not derive validity from the catalog under
// test.
constexpr unsigned implemented_count = 9;
constexpr bool canonical()
{
    ModuleLoadout a, b;
    return a.add(ModuleId::terminal_rotation) &&
           a.add(ModuleId::rotation_range) && b.add(ModuleId::rotation_range) &&
           b.add(ModuleId::terminal_rotation) && a == b && a.size() == 2 &&
           a.slots()[0] == ModuleId::rotation_range &&
           a.slots()[1] == ModuleId::terminal_rotation;
}
static_assert(canonical());
int main()
{
    for (unsigned first = 0; first <= 255; ++first)
        for (unsigned second = 0; second <= 255; ++second)
        {
            const bool valid = (first == 0 && second == 0) ||
                               (first >= 1 && first <= implemented_count &&
                                (second == 0 || (second > first &&
                                                 second <= implemented_count)));
            const auto decoded =
                decode_module_slots({static_cast<std::uint8_t>(first),
                                     static_cast<std::uint8_t>(second)});
            if (decoded.has_value() != valid)
                return 2;
            if (decoded)
            {
                const auto &slots = decoded->slots();
                if ((slots[0] ? static_cast<unsigned>(*slots[0]) : 0) !=
                        first ||
                    (slots[1] ? static_cast<unsigned>(*slots[1]) : 0) != second)
                    return 3;
            }
        }
    const ModuleLoadout empty;
    if (empty.size() != 0 || empty.slots()[0] || empty.slots()[1] ||
        empty.contains(ModuleId::rotation_range) || !canonical())
        return 1;
    for (unsigned raw = 0; raw <= 255; ++raw)
    {
        auto loadout = empty;
        const auto id = static_cast<ModuleId>(raw);
        const bool known = raw >= 1 && raw <= implemented_count;
        if (loadout.add(id) != known || (known && !loadout.contains(id)) ||
            (!known && loadout != empty))
            return 1;
        const auto before = loadout;
        if (loadout.add(id) || loadout != before)
        {
            std::cerr << "Rejected module mutated loadout\n";
            return 1;
        }
    }
    auto a = empty;
    if (!a.add(ModuleId::terminal_rotation))
        return 1;
    auto b = a;
    if (!a.add(ModuleId::rotation_range) || b.size() != 1 || a == b ||
        empty.size() != 0)
        return 1;
    const auto full = a;
    for (unsigned raw = 0; raw <= 255; ++raw)
        if (a.add(static_cast<ModuleId>(raw)) || a != full)
            return 1;
    return 0;
}

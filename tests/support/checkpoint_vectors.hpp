#pragma once
#include "application/checkpoint_format.hpp"
#include "application/run_checkpoint.hpp"

namespace closed_circuit::test
{
struct CheckpointVector
{
    RunCheckpoint checkpoint;
    CheckpointBytes bytes;
};
// Literal bytes calculated independently by tools/checkpoint/vectors.cjs 17,
// never by encoder.
inline constexpr auto checkpoint_vectors = []
{
    ModuleLoadout range, terminal, both;
    static_cast<void>(range.add(ModuleId::rotation_range));
    static_cast<void>(terminal.add(ModuleId::terminal_rotation));
    static_cast<void>(both.add(ModuleId::rotation_range));
    static_cast<void>(both.add(ModuleId::terminal_rotation));
    return std::array{
        CheckpointVector{{RunSeed{0},
                          *SectorNumber::from_value(1),
                          3,
                          2,
                          0,
                          {},
                          false,
                          StartConfigId::baseline},
                         {67, 67, 67,  80, 1,  0,  0, 0,  17,  0,
                          0,  0,  0,   0,  0,  0,  0, 0,  0,   0,
                          1,  3,  2,   0,  0,  0,  0, 0,  0,   0,
                          1,  0,  251, 71, 24, 12, 1, 10, 149, 249}},
        CheckpointVector{
            {RunSeed{UINT64_MAX}, *SectorNumber::from_value(sector_count), 1, 0,
             3, both, true, StartConfigId::baseline},
            {67,  67,  67,  80,  1,   0,   0,   0,  17,  0,   0,   0, 255, 255,
             255, 255, 255, 255, 255, 255, 6,   1,  0,   3,   0,   0, 0,   1,
             2,   1,   1,   0,   95,  118, 128, 27, 244, 216, 163, 13}},
        CheckpointVector{
            {RunSeed{0x0123456789abcdefull}, *SectorNumber::from_value(2), 2, 3,
             2, terminal, false, StartConfigId::baseline},
            {67,  67,  67,  80, 1,   0,  0,   0,  17, 0,  0,   0,  239, 205,
             171, 137, 103, 69, 35,  1,  2,   2,  3,  2,  0,   0,  0,   2,
             0,   0,   1,   0,  110, 80, 111, 88, 10, 67, 239, 225}},
        CheckpointVector{
            {RunSeed{1}, *SectorNumber::from_value(sector_count), 3, 1,
             UINT32_MAX - 3, range, true, StartConfigId::baseline},
            {67, 67, 67, 80, 1,   0,  0,  0,   17, 0,   0,   0,   1,   0,
             0,  0,  0,  0,  0,   0,  6,  3,   1,  252, 255, 255, 255, 1,
             0,  1,  1,  0,  131, 28, 98, 233, 76, 124, 7,   246}},
        CheckpointVector{{RunSeed{42}, *SectorNumber::from_value(1), 3, 0, 0,
                          range, false, StartConfigId::preinstalled_range},
                         {67, 67, 67, 80,  1,   0,   0,   0,   17, 0,
                          0,  0,  42, 0,   0,   0,   0,   0,   0,  0,
                          1,  3,  0,  0,   0,   0,   0,   1,   0,  0,
                          2,  0,  43, 247, 174, 146, 149, 115, 10, 40}},
        CheckpointVector{{RunSeed{7},
                          *SectorNumber::from_value(1),
                          3,
                          3,
                          2,
                          {},
                          false,
                          StartConfigId::charged_debt},
                         {67, 67, 67,  80, 1,   0,  0,   0,  17, 0,
                          0,  0,  7,   0,  0,   0,  0,   0,  0,  0,
                          1,  3,  3,   2,  0,   0,  0,   0,  0,  0,
                          3,  0,  205, 34, 244, 74, 137, 35, 78, 75}},
        // Since P087-04 byte 31 may carry the short_turn_limit bit.
        CheckpointVector{{RunSeed{9},
                          *SectorNumber::from_value(3),
                          2,
                          1,
                          1,
                          {},
                          false,
                          StartConfigId::baseline,
                          *DifficultyModifiers::from_value(1)},
                         {67, 67, 67, 80,  1,  0,   0,   0,   17,  0,
                          0,  0,  9,  0,   0,  0,   0,   0,   0,   0,
                          3,  2,  1,  1,   0,  0,   0,   0,   0,   0,
                          1,  1,  80, 175, 74, 127, 139, 143, 209, 113}},
        // Since P087-05 byte 31 may carry the lean_delivery bit.
        CheckpointVector{{RunSeed{11},
                          *SectorNumber::from_value(2),
                          3,
                          3,
                          0,
                          {},
                          false,
                          StartConfigId::charged_debt,
                          *DifficultyModifiers::from_value(2)},
                         {67, 67, 67,  80,  1,  0,  0,   0,  17, 0,
                          0,  0,  11,  0,   0,  0,  0,   0,  0,  0,
                          2,  3,  3,   0,   0,  0,  0,   0,  0,  0,
                          3,  2,  238, 103, 84, 57, 219, 28, 78, 234}},
        // Since P087-06 byte 31 may carry all three bits; a paid pulse.
        CheckpointVector{{RunSeed{13},
                          *SectorNumber::from_value(2),
                          3,
                          2,
                          1,
                          {},
                          true,
                          StartConfigId::charged_debt,
                          *DifficultyModifiers::from_value(7)},
                         {67, 67, 67,  80,  1,   0,   0,  0,   17, 0,
                          0,  0,  13,  0,   0,   0,   0,  0,   0,  0,
                          2,  3,  2,   1,   0,   0,   0,  0,   0,  1,
                          3,  7,  138, 232, 216, 120, 77, 199, 54, 86}}};
}();
} // namespace closed_circuit::test

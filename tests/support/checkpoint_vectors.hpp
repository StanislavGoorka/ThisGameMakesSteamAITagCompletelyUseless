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
// Literal bytes calculated independently by tools/checkpoint/vectors.cjs, never
// by encoder.
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
                         {67, 67, 67,  80,  1,   0,   0,   0,  16, 0,
                          0,  0,  0,   0,   0,   0,   0,   0,  0,  0,
                          1,  3,  2,   0,   0,   0,   0,   0,  0,  0,
                          1,  0,  234, 134, 221, 218, 204, 43, 7,  90}},
        CheckpointVector{
            {RunSeed{UINT64_MAX}, *SectorNumber::from_value(sector_count), 1, 0,
             3, both, true, StartConfigId::baseline},
            {67,  67,  67,  80,  1,   0,   0,  0, 16,  0,  0,   0,  255, 255,
             255, 255, 255, 255, 255, 255, 6,  1, 0,   3,  0,   0,  0,   1,
             2,   1,   1,   0,   90,  77,  62, 4, 209, 57, 170, 181}},
        CheckpointVector{
            {RunSeed{0x0123456789abcdefull}, *SectorNumber::from_value(2), 2, 3,
             2, terminal, false, StartConfigId::baseline},
            {67,  67,  67,  80, 1,   0,   0,   0,  16,  0,   0,  0,  239, 205,
             171, 137, 103, 69, 35,  1,   2,   2,  3,   2,   0,  0,  0,   2,
             0,   0,   1,   0,  203, 223, 100, 67, 181, 164, 94, 160}},
        CheckpointVector{
            {RunSeed{1}, *SectorNumber::from_value(sector_count), 3, 1,
             UINT32_MAX - 3, range, true, StartConfigId::baseline},
            {67, 67, 67, 80, 1,   0,  0,   0,  16,  0,   0,   0,   1,   0,
             0,  0,  0,  0,  0,   0,  6,   3,  1,   252, 255, 255, 255, 1,
             0,  1,  1,  0,  178, 96, 160, 63, 189, 139, 28,  86}},
        CheckpointVector{{RunSeed{42}, *SectorNumber::from_value(1), 3, 0, 0,
                          range, false, StartConfigId::preinstalled_range},
                         {67, 67, 67,  80, 1,   0,   0,   0,   16,  0,
                          0,  0,  42,  0,  0,   0,   0,   0,   0,   0,
                          1,  3,  0,   0,  0,   0,   0,   1,   0,   0,
                          2,  0,  142, 75, 103, 107, 213, 131, 137, 116}},
        CheckpointVector{{RunSeed{7},
                          *SectorNumber::from_value(1),
                          3,
                          3,
                          2,
                          {},
                          false,
                          StartConfigId::charged_debt},
                         {67, 67, 67,  80,  1,  0,   0,   0,  16, 0,
                          0,  0,  7,   0,   0,  0,   0,   0,  0,  0,
                          1,  3,  3,   2,   0,  0,   0,   0,  0,  0,
                          3,  0,  248, 182, 14, 100, 194, 64, 48, 221}}};
}();
} // namespace closed_circuit::test

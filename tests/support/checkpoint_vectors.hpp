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
        CheckpointVector{
            {RunSeed{0}, *SectorNumber::from_value(1), 3, 2, 0, {}, false},
            {67, 67, 67, 80, 1,  0,  0,   0,   8,   0,  0,   0, 0, 0,
             0,  0,  0,  0,  0,  0,  1,   3,   2,   0,  0,   0, 0, 0,
             0,  0,  0,  0,  11, 23, 145, 239, 238, 71, 205, 36}},
        CheckpointVector{
            {RunSeed{UINT64_MAX}, *SectorNumber::from_value(sector_count), 1, 0,
             3, both, true},
            {67,  67,  67,  80,  1,   0,   0,   0,  8,  0,  0,   0,  255, 255,
             255, 255, 255, 255, 255, 255, 6,   1,  0,  3,  0,   0,  0,   1,
             2,   1,   0,   0,   123, 163, 218, 33, 38, 74, 215, 203}},
        CheckpointVector{
            {RunSeed{0x0123456789abcdefull}, *SectorNumber::from_value(2), 2, 3,
             2, terminal, false},
            {67,  67,  67,  80, 1,  0,   0,  0,  8,   0,   0,  0, 239, 205,
             171, 137, 103, 69, 35, 1,   2,  2,  3,   2,   0,  0, 0,   2,
             0,   0,   0,   0,  26, 133, 14, 38, 104, 208, 18, 82}},
        CheckpointVector{{RunSeed{1}, *SectorNumber::from_value(sector_count),
                          3, 1, UINT32_MAX - 3, range, true},
                         {67, 67, 67,  80,  1,   0,   0,   0,   8,   0,
                          0,  0,  1,   0,   0,   0,   0,   0,   0,   0,
                          6,  3,  1,   252, 255, 255, 255, 1,   0,   1,
                          0,  0,  211, 46,  80,  250, 12,  238, 209, 185}}};
}();
} // namespace closed_circuit::test

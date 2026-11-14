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
            {67, 67, 67, 80, 1,   0,   0,   0,   4,   0,   0,  0,  0, 0,
             0,  0,  0,  0,  0,   0,   1,   3,   2,   0,   0,  0,  0, 0,
             0,  0,  0,  0,  135, 109, 233, 249, 255, 162, 46, 138}},
        CheckpointVector{
            {RunSeed{UINT64_MAX}, *SectorNumber::from_value(sector_count), 1, 0,
             3, both, true},
            {67,  67,  67,  80,  1,   0,   0,   0,   4,  0,  0,   0,  255, 255,
             255, 255, 255, 255, 255, 255, 6,   1,   0,  3,  0,   0,  0,   1,
             2,   1,   0,   0,   111, 184, 186, 109, 26, 89, 130, 233}},
        CheckpointVector{
            {RunSeed{0x0123456789abcdefull}, *SectorNumber::from_value(2), 2, 3,
             2, terminal, false},
            {67,  67,  67,  80, 1,   0,  0,  0,  4,   0,   0,   0, 239, 205,
             171, 137, 103, 69, 35,  1,  2,  2,  3,   2,   0,   0, 0,   2,
             0,   0,   0,   0,  118, 61, 25, 90, 255, 200, 173, 77}},
        CheckpointVector{{RunSeed{1}, *SectorNumber::from_value(sector_count),
                          3, 1, UINT32_MAX - 3, range, true},
                         {67, 67, 67,  80,  1,   0,   0,   0,   4,   0,
                          0,  0,  1,   0,   0,   0,   0,   0,   0,   0,
                          6,  3,  1,   252, 255, 255, 255, 1,   0,   1,
                          0,  0,  199, 223, 142, 74,  138, 135, 176, 197}}};
}();
} // namespace closed_circuit::test

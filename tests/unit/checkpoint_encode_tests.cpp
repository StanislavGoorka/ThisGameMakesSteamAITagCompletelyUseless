#include "../support/checkpoint_vectors.hpp"
#include "application/checkpoint_encode.hpp"
#include <iostream>
using namespace closed_circuit;
static_assert(encode_checkpoint(test::checkpoint_vectors[0].checkpoint) ==
              test::checkpoint_vectors[0].bytes);
namespace
{
bool cases()
{
    for (const auto &v : test::checkpoint_vectors)
    {
        const auto before = v.checkpoint;
        for (int repeat = 0; repeat < 3; ++repeat)
        {
            auto bytes = encode_checkpoint(v.checkpoint);
            if (!bytes || *bytes != v.bytes || v.checkpoint != before)
                return false;
            bytes->fill(255);
            if (encode_checkpoint(v.checkpoint) != v.bytes)
                return false;
        }
        for (int field = 0; field < 7; ++field)
        {
            auto invalid = before;
            if (field == 0)
                invalid.player_integrity = 0;
            if (field == 1)
                invalid.player_integrity = 4;
            if (field == 2)
                invalid.player_integrity = 255;
            if (field == 3)
                invalid.reserve = 4;
            if (field == 4)
                invalid.reserve = 255;
            if (field == 5)
            {
                invalid.overload_debt = 3;
                invalid.entry_debt_pulse = false;
            }
            if (field == 6)
            {
                invalid.overload_debt = UINT32_MAX - 2;
                invalid.entry_debt_pulse = true;
            }
            const auto snapshot = invalid;
            for (int repeat = 0; repeat < 2; ++repeat)
                if (encode_checkpoint(invalid) || invalid != snapshot ||
                    encode_checkpoint(before) != v.bytes)
                    return false;
        }
    }
    return true;
}
} // namespace
int main()
{
    if (!cases())
    {
        std::cerr << "Checkpoint encoding vector mismatch\n";
        return 1;
    }
    return 0;
}

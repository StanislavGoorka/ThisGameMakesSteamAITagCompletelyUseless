#include "../support/checkpoint_bytes.hpp"
#include "../support/checkpoint_vectors.hpp"
#include "application/checkpoint_decode.hpp"
#include "application/checkpoint_encode.hpp"
#include <iostream>
using namespace closed_circuit;
static_assert(decode_checkpoint(test::checkpoint_vectors[0].bytes) ==
              test::checkpoint_vectors[0].checkpoint);
namespace
{
bool rejected(std::span<const std::uint8_t> bytes)
{
    const std::vector<std::uint8_t> before(bytes.begin(), bytes.end());
    return !decode_checkpoint(bytes) && std::ranges::equal(bytes, before);
}
bool cases()
{
    for (const auto &v : test::checkpoint_vectors)
    {
        auto sealed = v.bytes;
        test::reseal(sealed);
        if (sealed != v.bytes)
            return false;
        for (int repeat = 0; repeat < 3; ++repeat)
        {
            const auto decoded = decode_checkpoint(v.bytes);
            if (!decoded || *decoded != v.checkpoint ||
                encode_checkpoint(*decoded) != v.bytes)
                return false;
        }
        if (!rejected({}) ||
            !rejected(std::span<const std::uint8_t>{v.bytes}.first(39)))
            return false;
        auto extra = std::vector<std::uint8_t>(v.bytes.begin(), v.bytes.end());
        extra.push_back(0);
        if (!rejected(extra))
            return false;
        for (int scenario = 0; scenario < 17; ++scenario)
        {
            auto bad = v.bytes;
            if (scenario == 0)
                bad[0] = 0;
            if (scenario == 1)
                bad[4] = 2;
            if (scenario == 2)
                bad[8] = 2;
            if (scenario == 3)
                bad[20] = 0;
            if (scenario == 4)
                bad[20] = 4;
            if (scenario == 5)
                bad[21] = 0;
            if (scenario == 6)
                bad[21] = 4;
            if (scenario == 7)
                bad[22] = 4;
            if (scenario == 8)
                bad[29] = 2;
            if (scenario == 9)
                bad[30] = 1;
            if (scenario == 10)
                bad[31] = 1;
            if (scenario == 11)
            {
                bad[27] = 0;
                bad[28] = 1;
            }
            if (scenario == 12)
            {
                bad[27] = 1;
                bad[28] = 1;
            }
            if (scenario == 13)
            {
                bad[27] = 2;
                bad[28] = 1;
            }
            if (scenario == 14)
                bad[27] = 3;
            if (scenario == 15)
            {
                bad[23] = 3;
                bad[24] = bad[25] = bad[26] = 0;
                bad[29] = 0;
            }
            if (scenario == 16)
            {
                bad[23] = 254;
                bad[24] = bad[25] = bad[26] = 255;
                bad[29] = 1;
            }
            test::reseal(bad);
            if (!rejected(bad))
                return false;
        }
        auto corrupt = v.bytes;
        corrupt[39] ^= 1;
        if (!rejected(corrupt) || decode_checkpoint(v.bytes) != v.checkpoint)
            return false;
    }
    return true;
}
} // namespace
int main()
{
    if (!cases())
    {
        std::cerr << "Checkpoint decoding contract mismatch\n";
        return 1;
    }
    return 0;
}

#include "../support/profile_vectors.hpp"
#include "application/profile_decode.hpp"
#include "application/profile_encode.hpp"
#include <vector>
using namespace closed_circuit;

static_assert(decode_profile(test::profile_vectors[0]) == LocalProfile{});

// Independent test checksum; never invokes the production checksum helper.
void renew_checksum(ProfileBytes &bytes)
{
    std::uint64_t hash = 0xcbf29ce484222325ull;
    for (unsigned i = 0; i != 16; ++i)
    {
        hash ^= bytes[i];
        hash *= 0x100000001b3ull;
    }
    for (unsigned i = 0; i != 8; ++i)
    {
        bytes[16 + i] = static_cast<std::uint8_t>(hash & 255);
        hash /= 256;
    }
}

int main()
{
    for (unsigned a = 0; a <= 31; ++a)
        for (unsigned v = 0; v <= 3; ++v)
            for (unsigned volume = 0; volume <= 100; ++volume)
                for (unsigned mute = 0; mute != 2; ++mute)
                {
                    const LocalProfile profile{
                        static_cast<std::uint16_t>(a),
                        static_cast<std::uint8_t>(v),
                        static_cast<std::uint8_t>(volume), mute == 1};
                    const auto bytes = encode_profile(profile);
                    if (!bytes || decode_profile(*bytes) != profile)
                        return 1;
                }
    for (std::size_t i = 0; i < test::profile_vectors.size(); ++i)
    {
        const auto &fixture = test::profile_vectors[i];
        if (decode_profile(fixture) != test::profile_values[i])
            return 1;
        for (std::size_t size = 0; size < 24; ++size)
            if (decode_profile(std::span{fixture}.first(size)))
                return 1;
        for (unsigned extra = 1; extra <= 64; ++extra)
        {
            std::vector<std::uint8_t> bytes(fixture.begin(), fixture.end());
            bytes.resize(24 + extra);
            if (decode_profile(bytes))
                return 1;
        }
        for (unsigned offset = 0; offset < 24; ++offset)
            for (unsigned raw = 0; raw <= 255; ++raw)
            {
                auto bytes = fixture;
                bytes[offset] = static_cast<std::uint8_t>(raw);
                if (decode_profile(bytes).has_value() != (bytes == fixture))
                    return 1;
                if (offset >= 16)
                    continue;
                renew_checksum(bytes);
                const bool valid = offset == 8    ? raw <= 31
                                   : offset == 10 ? raw <= 3
                                   : offset == 11 ? raw <= 100
                                   : offset == 12 ? raw <= 1
                                                  : raw == fixture[offset];
                const auto result = decode_profile(bytes);
                if (result.has_value() != valid ||
                    (result && encode_profile(*result) != bytes))
                    return 1;
            }
    }
    return 0;
}

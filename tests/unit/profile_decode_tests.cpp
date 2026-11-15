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

// Size, extra data and every single-byte change of one valid fixture. With a
// renewed checksum a change is accepted only within the fixture's schema:
// schema 1 holds achievement bits 0..4, schema 2 bits 0..11.
bool check_fixture(const ProfileBytes &fixture, const LocalProfile &expected)
{
    if (decode_profile(fixture) != expected)
        return false;
    for (std::size_t size = 0; size < 24; ++size)
        if (decode_profile(std::span{fixture}.first(size)))
            return false;
    for (unsigned extra = 1; extra <= 64; ++extra)
    {
        std::vector<std::uint8_t> bytes(fixture.begin(), fixture.end());
        bytes.resize(24 + extra);
        if (decode_profile(bytes))
            return false;
    }
    for (unsigned offset = 0; offset < 24; ++offset)
        for (unsigned raw = 0; raw <= 255; ++raw)
        {
            auto bytes = fixture;
            bytes[offset] = static_cast<std::uint8_t>(raw);
            if (decode_profile(bytes).has_value() != (bytes == fixture))
                return false;
            if (offset >= 16)
                continue;
            renew_checksum(bytes);
            const unsigned bits = bytes[8] | (bytes[9] << 8);
            const unsigned limit = bytes[4] == 1 ? 0x1fu : 0x0fffu;
            const bool known =
                offset == 4 ? raw == 1 || raw == 2 : raw == fixture[offset];
            const bool valid = offset == 8 || offset == 9 ? bits <= limit
                               : offset == 4  ? known && bits <= limit
                               : offset == 10 ? raw <= 3
                               : offset == 11 ? raw <= 100
                               : offset == 12 ? raw <= 1
                                              : known;
            const auto result = decode_profile(bytes);
            if (result.has_value() != valid)
                return false;
            // A valid schema 2 record re-encodes to itself.
            if (result && bytes[4] == 2 && encode_profile(*result) != bytes)
                return false;
        }
    return true;
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
    // Every format bit, independent of the other fields.
    for (unsigned a = 0; a <= 0x0fff; ++a)
    {
        const LocalProfile profile{static_cast<std::uint16_t>(a), 1, 50, false};
        const auto bytes = encode_profile(profile);
        if (!bytes || decode_profile(*bytes) != profile)
            return 1;
    }
    if (decode_profile(test::profile_wide_vector) != test::profile_wide_value ||
        decode_profile(test::profile_wide_v1_vector))
        return 1;
    for (std::size_t i = 0; i < test::profile_vectors.size(); ++i)
    {
        if (!check_fixture(test::profile_vectors[i], test::profile_values[i]) ||
            !check_fixture(test::profile_v1_vectors[i],
                           test::profile_values[i]))
            return 1;
        // Reading schema 1 yields the value; writing it yields schema 2.
        if (encode_profile(test::profile_values[i]) != test::profile_vectors[i])
            return 1;
    }
    if (!check_fixture(test::profile_wide_vector, test::profile_wide_value))
        return 1;
    return 0;
}

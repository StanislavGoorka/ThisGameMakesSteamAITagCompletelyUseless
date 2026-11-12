#include "../support/profile_vectors.hpp"
#include "application/profile_encode.hpp"
using namespace closed_circuit;

static_assert(encode_profile(LocalProfile{}) == test::profile_vectors[0]);

int main()
{
    for (std::size_t i = 0; i < test::profile_values.size(); ++i)
    {
        const auto profile = test::profile_values[i];
        if (encode_profile(profile) != test::profile_vectors[i] ||
            profile != test::profile_values[i])
            return 1;
    }
    for (unsigned raw = 0; raw <= 65535; ++raw)
    {
        LocalProfile profile;
        profile.achievement_bits = static_cast<std::uint16_t>(raw);
        const auto before = profile;
        const auto bytes = encode_profile(profile);
        if (bytes.has_value() != (raw <= 31) || profile != before ||
            (bytes && ((*bytes)[8] != raw || (*bytes)[9] != 0)))
            return 1;
    }
    for (unsigned raw = 0; raw <= 255; ++raw)
    {
        LocalProfile profile;
        profile.master_volume = static_cast<std::uint8_t>(raw);
        if (encode_profile(profile).has_value() != (raw <= 100))
            return 1;
        profile = {};
        profile.variant_bits = static_cast<std::uint8_t>(raw);
        if (encode_profile(profile).has_value() != (raw <= 3))
            return 1;
    }
    return 0;
}

#include "application/rotation_binding.hpp"
#include <algorithm>
#include <limits>
using namespace closed_circuit;

int main()
{
    constexpr RotationBinding default_binding;
    static_assert(default_binding.letter() == 'R');
    static_assert(!RotationBinding::from_letter('O'));
    static_assert(
        !RotationBinding::from_letter(std::numeric_limits<unsigned>::max()));
    constexpr RotationBindingBytes canonical{0x43, 0x43, 0x4b, 0x42, 1,    0x52,
                                             0,    0,    0xe5, 0x33, 0xa5, 0xcf,
                                             0x5e, 0x39, 0xd6, 0x7f};
    const auto encoded = encode_rotation_binding(default_binding);
    if (encoded != canonical ||
        decode_rotation_binding(canonical) != default_binding)
        return 1;
    for (unsigned raw = 0; raw < 256; ++raw)
    {
        const auto value = RotationBinding::from_letter(raw);
        if (value.has_value() != (raw >= 65 && raw <= 90 && raw != 79))
            return 2;
        if (value &&
            (value->letter() != raw ||
             decode_rotation_binding(encode_rotation_binding(*value)) != value))
            return 3;
    }
    for (std::size_t size = 0; size < encoded.size(); ++size)
        if (decode_rotation_binding(std::span{encoded}.first(size)))
            return 4;
    std::array<std::uint8_t, 17> extra{};
    std::copy(encoded.begin(), encoded.end(), extra.begin());
    if (decode_rotation_binding(extra))
        return 5;
    for (std::size_t i = 0; i < encoded.size(); ++i)
        for (unsigned bit = 0; bit < 8; ++bit)
        {
            auto broken = encoded;
            broken[i] ^= static_cast<std::uint8_t>(1U << bit);
            if (decode_rotation_binding(broken))
                return 6;
        }
    // Recompute checksum independently after corrupting semantic fields.
    for (auto index : {0, 1, 2, 3, 4, 5, 6, 7})
    {
        auto broken = encoded;
        broken[index] = index == 5 ? 'O' : 255;
        std::uint64_t hash = 14695981039346656037ULL;
        for (unsigned i = 0; i < 8; ++i)
        {
            hash ^= broken[i];
            hash *= 1099511628211ULL;
        }
        for (unsigned i = 0; i < 8; ++i)
        {
            broken[8 + i] = static_cast<std::uint8_t>(hash & 255);
            hash >>= 8;
        }
        if (decode_rotation_binding(broken))
            return 7;
    }
    return 0;
}

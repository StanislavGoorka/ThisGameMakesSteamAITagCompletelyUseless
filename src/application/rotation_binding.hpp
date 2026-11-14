#pragma once
#include <array>
#include <cstdint>
#include <optional>
#include <span>

namespace closed_circuit
{
class RotationBinding
{
  public:
    constexpr RotationBinding() noexcept = default;
    [[nodiscard]] static constexpr std::optional<RotationBinding>
    from_letter(unsigned letter) noexcept
    {
        if (letter < 'A' || letter > 'Z' || letter == 'O')
            return std::nullopt;
        RotationBinding result;
        result.letter_ = static_cast<std::uint8_t>(letter);
        return result;
    }
    [[nodiscard]] constexpr std::uint8_t letter() const noexcept
    {
        return letter_;
    }
    constexpr bool operator==(const RotationBinding &) const noexcept = default;

  private:
    std::uint8_t letter_ = 'R';
};

using RotationBindingBytes = std::array<std::uint8_t, 16>;
namespace rotation_binding_detail
{
[[nodiscard]] constexpr std::uint64_t
checksum(std::span<const std::uint8_t> bytes) noexcept
{
    std::uint64_t hash = 14695981039346656037ULL;
    for (const auto byte : bytes)
        hash = (hash ^ byte) * 1099511628211ULL;
    return hash;
}
} // namespace rotation_binding_detail

[[nodiscard]] constexpr RotationBindingBytes
encode_rotation_binding(RotationBinding binding) noexcept
{
    RotationBindingBytes bytes{0x43, 0x43, 0x4b, 0x42, 1, binding.letter()};
    const auto hash = rotation_binding_detail::checksum(
        std::span<const std::uint8_t>{bytes}.first(8));
    for (unsigned i = 0; i < 8; ++i)
        bytes[8 + i] = static_cast<std::uint8_t>(hash >> (8 * i));
    return bytes;
}
[[nodiscard]] constexpr std::optional<RotationBinding>
decode_rotation_binding(std::span<const std::uint8_t> bytes) noexcept
{
    if (bytes.size() != 16 || bytes[0] != 0x43 || bytes[1] != 0x43 ||
        bytes[2] != 0x4b || bytes[3] != 0x42 || bytes[4] != 1 ||
        bytes[6] != 0 || bytes[7] != 0)
        return std::nullopt;
    const auto binding = RotationBinding::from_letter(bytes[5]);
    if (!binding)
        return std::nullopt;
    const auto expected = encode_rotation_binding(*binding);
    for (unsigned i = 8; i < 16; ++i)
        if (expected[i] != bytes[i])
            return std::nullopt;
    return binding;
}
} // namespace closed_circuit

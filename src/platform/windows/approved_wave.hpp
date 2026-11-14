#pragma once
#include "application/pcm_wave.hpp"
#include "platform/windows/asset_read.hpp"

namespace closed_circuit::windows
{
[[nodiscard]] inline const AssetReference &delivery_audio_reference()
{
    static const auto reference = *AssetReference::from_fields(
        "comigo4947244-powerup01",
        "assets/approved/comigo-bleeps-4947244/PowerUp_01.wav",
        "85D03C812DD83C7D264D2EF6450508190F99BBBC9757170DC1CB22788D5F00F7",
        71836);
    return reference;
}

// Every instance comes from the pinned, hash-verified approved file. It owns
// its bytes, so copying/moving it never copies a pointer into another owner.
class ApprovedDeliveryWave
{
  public:
    [[nodiscard]] static std::optional<ApprovedDeliveryWave>
    load(const std::filesystem::path &root)
    {
        if (!root.is_absolute())
            return std::nullopt;
        auto read = read_asset_file(root, delivery_audio_reference());
        if (!read.bytes || !decode_pcm16_wave(*read.bytes))
            return std::nullopt;
        return ApprovedDeliveryWave{std::move(*read.bytes)};
    }
    // A moved-from owner returns an empty view. Other views are borrowed
    // until destruction, assignment or moving from this owner.
    [[nodiscard]] Pcm16WaveView view() const noexcept
    {
        return decode_pcm16_wave(bytes_).value_or(Pcm16WaveView{});
    }

  private:
    explicit ApprovedDeliveryWave(std::vector<std::uint8_t> bytes) noexcept
        : bytes_(std::move(bytes))
    {
    }
    std::vector<std::uint8_t> bytes_;
};
} // namespace closed_circuit::windows

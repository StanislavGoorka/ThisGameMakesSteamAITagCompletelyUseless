#pragma once
#include <cstdint>
#include <optional>
#include <span>
#include <string_view>

namespace closed_circuit
{
// Borrowed view, valid only while the unmodified input buffer is alive.
// Only interleaved, little-endian PCM16 mono/stereo is accepted.
struct Pcm16WaveView
{
    std::uint16_t channels = 0;
    std::uint32_t sample_rate = 0;
    std::uint16_t block_align = 0;
    std::span<const std::uint8_t> data;
};

[[nodiscard]] inline std::optional<Pcm16WaveView>
decode_pcm16_wave(std::span<const std::uint8_t> bytes) noexcept
{
    if (bytes.size() < 12 || bytes.size() > 1024 * 1024)
        return std::nullopt;
    const auto id_is = [&](std::size_t at, std::string_view id)
    {
        for (std::size_t i = 0; i < 4; ++i)
            if (bytes[at + i] != static_cast<std::uint8_t>(id[i]))
                return false;
        return true;
    };
    const auto u16 = [&](std::size_t at)
    { return static_cast<std::uint16_t>(bytes[at] | (bytes[at + 1] << 8)); };
    const auto u32 = [&](std::size_t at)
    {
        return std::uint32_t{bytes[at]} | (std::uint32_t{bytes[at + 1]} << 8) |
               (std::uint32_t{bytes[at + 2]} << 16) |
               (std::uint32_t{bytes[at + 3]} << 24);
    };
    if (!id_is(0, "RIFF") || !id_is(8, "WAVE") || u32(4) != bytes.size() - 8)
        return std::nullopt;
    Pcm16WaveView view;
    bool format = false, data = false;
    unsigned chunks = 0;
    for (std::size_t at = 12; at < bytes.size();)
    {
        if (bytes.size() - at < 8 || ++chunks > 256)
            return std::nullopt;
        const auto size = u32(at + 4);
        const auto begin = at + 8;
        if (size > bytes.size() - begin)
            return std::nullopt;
        if (id_is(at, "fmt "))
        {
            if (format || (size != 16 && size != 18) || u16(begin) != 1 ||
                (size == 18 && u16(begin + 16) != 0))
                return std::nullopt;
            format = true;
            view.channels = u16(begin + 2);
            view.sample_rate = u32(begin + 4);
            view.block_align = u16(begin + 12);
            if ((view.channels != 1 && view.channels != 2) ||
                view.sample_rate < 8000 || view.sample_rate > 192000 ||
                u16(begin + 14) != 16 ||
                view.block_align != view.channels * 2 ||
                u32(begin + 8) != view.sample_rate * view.block_align)
                return std::nullopt;
        }
        else if (id_is(at, "data"))
        {
            if (data || !size)
                return std::nullopt;
            data = true;
            view.data = bytes.subspan(begin, size);
        }
        at = begin + size;
        if (size & 1)
        {
            if (at == bytes.size())
                return std::nullopt;
            ++at;
        }
    }
    if (!format || !data || view.data.size() % view.block_align)
        return std::nullopt;
    return view;
}
} // namespace closed_circuit

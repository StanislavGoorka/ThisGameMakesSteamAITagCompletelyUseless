#include "application/pcm_wave.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <vector>
using namespace closed_circuit;
using Bytes = std::vector<std::uint8_t>;

// Only metadata is changed. Sample data always comes from the approved file.
void set32(Bytes &bytes, std::size_t at, std::uint32_t value)
{
    for (unsigned i = 0; i < 4; ++i)
        bytes[at + i] = static_cast<std::uint8_t>(value >> (i * 8));
}
void size_riff(Bytes &bytes)
{
    set32(bytes, 4, static_cast<std::uint32_t>(bytes.size() - 8));
}
void metadata(Bytes &bytes, std::size_t length)
{
    bytes.insert(bytes.end(), {'J', 'U', 'N', 'K'});
    const auto at = bytes.size();
    bytes.resize(at + 4);
    set32(bytes, at, static_cast<std::uint32_t>(length));
    bytes.insert(bytes.end(), length, 'm');
    if (length & 1)
        bytes.push_back(0);
    size_riff(bytes);
}
bool same_audio(const Pcm16WaveView &wave, const Bytes &original)
{
    return wave.channels == 1 && wave.sample_rate == 44100 &&
           wave.block_align == 2 && wave.data.size() == 71792 &&
           std::ranges::equal(wave.data, std::span{original}.subspan(44));
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 1;
    std::ifstream file(
        std::filesystem::path{argv[1]} /
            "assets/approved/comigo-bleeps-4947244/PowerUp_01.wav",
        std::ios::binary);
    const Bytes original{std::istreambuf_iterator<char>{file}, {}};
    if (original.size() != 71836)
        return 2;
    const auto saved = original;
    const auto parsed = decode_pcm16_wave(original);
    if (!parsed || !same_audio(*parsed, original) ||
        parsed->data.data() != original.data() + 44)
        return 3;
    for (auto rate : {8000u, 192000u})
    {
        auto stereo = original;
        stereo[22] = 2;
        stereo[32] = 4;
        set32(stereo, 24, rate);
        set32(stereo, 28, rate * 4);
        const auto view = decode_pcm16_wave(stereo);
        if (!view || view->channels != 2 || view->sample_rate != rate ||
            view->block_align != 4 ||
            !std::ranges::equal(view->data, parsed->data))
            return 18;
    }
    for (std::size_t size = 0; size < original.size(); ++size)
        if (decode_pcm16_wave(std::span{original}.first(size)))
            return 4;
    for (auto offset : {0u, 8u, 12u, 36u})
    {
        auto bad = original;
        bad[offset] = '?';
        if (decode_pcm16_wave(bad))
            return 5;
    }
    for (auto [offset, value] : {std::pair{4u, 0u},
                                 {4u, 0xffffffffu},
                                 {16u, 15u},
                                 {16u, 0xffffffffu},
                                 {20u, 0u},
                                 {20u, 3u},
                                 {22u, 0u},
                                 {22u, 3u},
                                 {24u, 7999u},
                                 {24u, 192001u},
                                 {28u, 1u},
                                 {32u, 0u},
                                 {34u, 8u},
                                 {40u, 0u},
                                 {40u, 71791u},
                                 {40u, 0xffffffffu}})
    {
        auto bad = original;
        // 16-bit fields must not overwrite the next 16-bit field.
        if (offset == 20 || offset == 22 || offset == 32 || offset == 34)
        {
            bad[offset] = static_cast<std::uint8_t>(value);
            bad[offset + 1] = 0;
        }
        else
            set32(bad, offset, value);
        if (offset == 24)
            set32(bad, 28, value * 2);
        if (decode_pcm16_wave(bad))
            return 6;
    }
    auto extended = original;
    extended.insert(extended.begin() + 36, 2, 0);
    set32(extended, 16, 18);
    size_riff(extended);
    if (!decode_pcm16_wave(extended) ||
        !same_audio(*decode_pcm16_wave(extended), original))
        return 7;
    extended[36] = 1;
    if (decode_pcm16_wave(extended))
        return 8;
    auto reordered = original;
    std::rotate(reordered.begin() + 12, reordered.begin() + 36,
                reordered.end());
    if (!decode_pcm16_wave(reordered) ||
        !same_audio(*decode_pcm16_wave(reordered), original))
        return 9;
    auto duplicate = original;
    duplicate.insert(duplicate.end(), original.begin() + 12,
                     original.begin() + 36);
    size_riff(duplicate);
    if (decode_pcm16_wave(duplicate))
        return 10;
    duplicate = original;
    duplicate.insert(duplicate.end(), {'d', 'a', 't', 'a', 0, 0, 0, 0});
    size_riff(duplicate);
    if (decode_pcm16_wave(duplicate))
        return 11;
    auto padded = original;
    metadata(padded, 1);
    if (!decode_pcm16_wave(padded) ||
        !same_audio(*decode_pcm16_wave(padded), original))
        return 12;
    padded.pop_back();
    size_riff(padded);
    if (decode_pcm16_wave(padded))
        return 13;
    auto many = original;
    for (unsigned i = 0; i < 254; ++i)
        metadata(many, 0);
    if (!decode_pcm16_wave(many))
        return 14;
    metadata(many, 0);
    if (decode_pcm16_wave(many))
        return 15;
    auto large = original;
    metadata(large, 1024 * 1024 - original.size() - 8);
    if (!decode_pcm16_wave(large) ||
        !same_audio(*decode_pcm16_wave(large), original))
        return 16;
    metadata(large, 0);
    if (decode_pcm16_wave(large) || original != saved)
        return 17;
    return 0;
}

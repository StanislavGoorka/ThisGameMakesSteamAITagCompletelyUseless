#pragma once
#include "platform/windows/xaudio2_playback.hpp"
#include "xaudio2_fake.hpp"
namespace closed_circuit::test
{
class FakeXAudio2VoiceApi final : public windows::XAudio2VoiceApi
{
  public:
    FakeXAudio2VoiceApi(test::FakeXAudio2Api &device,
                        std::span<const std::uint8_t> expected)
        : device(device), expected(expected)
    {
    }
    HRESULT create_result = S_OK, submit_result = S_OK, start_result = S_OK;
    bool null_source = false, partial_source = false, valid = true;
    unsigned creates = 0, submits = 0, starts = 0, destroys = 0, queued = 0;
    HRESULT create_source(IXAudio2 *engine, const WAVEFORMATEX &format,
                          IXAudio2SourceVoice **out) noexcept override
    {
        ++creates;
        valid &= engine && device.callback && device.live_sources == 0 &&
                 format.wFormatTag == WAVE_FORMAT_PCM &&
                 format.nChannels == 1 && format.nSamplesPerSec == 44100 &&
                 format.nAvgBytesPerSec == 88200 && format.nBlockAlign == 2 &&
                 format.wBitsPerSample == 16 && format.cbSize == 0;
        *out = !null_source && (SUCCEEDED(create_result) || partial_source)
                   ? pointer()
                   : nullptr;
        if (*out)
            ++device.live_sources;
        return create_result;
    }
    HRESULT submit(IXAudio2SourceVoice *voice,
                   const XAUDIO2_BUFFER &buffer) noexcept override
    {
        ++submits;
        valid &= voice == pointer() && device.live_sources == 1 &&
                 buffer.Flags == XAUDIO2_END_OF_STREAM &&
                 buffer.AudioBytes == expected.size() &&
                 buffer.PlayBegin == 0 && buffer.PlayLength == 0 &&
                 buffer.LoopBegin == 0 && buffer.LoopLength == 0 &&
                 buffer.LoopCount == 0 && buffer.pContext == nullptr;
        valid &= std::ranges::equal(
            std::span{buffer.pAudioData, buffer.AudioBytes}, expected);
        if (pending)
            valid &= pending == buffer.pAudioData;
        pending = buffer.pAudioData;
        if (SUCCEEDED(submit_result))
            ++queued;
        return submit_result;
    }
    HRESULT start(IXAudio2SourceVoice *voice) noexcept override
    {
        ++starts;
        valid &= voice == pointer() && queued && !started;
        started = SUCCEEDED(start_result);
        return start_result;
    }
    void destroy_source(IXAudio2SourceVoice *voice) noexcept override
    {
        ++destroys;
        valid &= voice == pointer() && device.live_sources == 1;
        // DestroyVoice must finish reading before the owner frees its data.
        if (pending)
            valid &= std::ranges::equal(std::span{pending, expected.size()},
                                        expected);
        pending = nullptr;
        queued = 0;
        started = false;
        --device.live_sources;
    }

  private:
    IXAudio2SourceVoice *pointer() noexcept
    {
        return reinterpret_cast<IXAudio2SourceVoice *>(&token);
    }
    test::FakeXAudio2Api &device;
    std::span<const std::uint8_t> expected;
    const std::uint8_t *pending = nullptr;
    bool started = false;
    int token = 0;
};

} // namespace closed_circuit::test

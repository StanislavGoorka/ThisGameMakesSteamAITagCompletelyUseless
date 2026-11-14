#include "../support/xaudio2_voice_fake.hpp"
#include "platform/windows/xaudio2_playback.hpp"
#include <chrono>
#include <iostream>
#include <thread>
#include <type_traits>
using namespace closed_circuit;
using Playback = windows::XAudio2Playback;
static_assert(!std::is_copy_constructible_v<Playback>);
static_assert(!std::is_move_constructible_v<Playback>);

using FakeVoiceApi = test::FakeXAudio2VoiceApi;

bool fake_lifecycle(const windows::ApprovedDeliveryWave &wave)
{
    test::FakeXAudio2Api device;
    FakeVoiceApi voice{device, wave.view().data};
    {
        Playback player{wave, device, voice};
        if (!player.available() || player.error() != S_OK || voice.creates != 1)
            return false;
        for (int repeat = 0; repeat < 3; ++repeat)
            if (player.play() != AudioPlayResult::played)
                return false;
        if (voice.submits != 3 || voice.starts != 1 || voice.queued != 3)
            return false;
        std::thread worker{[cb = device.callback] {
            cb->OnCriticalError(XAUDIO2_E_DEVICE_INVALIDATED);
        }};
        worker.join();
        if (player.available() ||
            player.error() != XAUDIO2_E_DEVICE_INVALIDATED || !voice.queued)
            return false;
        if (player.play() != AudioPlayResult::unavailable || voice.queued ||
            device.live_sources)
            return false;
        if (player.recreate() != S_OK || !player.available() ||
            player.play() != AudioPlayResult::played)
            return false;
        // Also recreate with a live, queued source: it must go first.
        if (player.recreate() != S_OK || !player.available() || voice.queued)
            return false;
        if (player.play() != AudioPlayResult::played)
            return false;
    }
    return voice.valid && device.valid && !device.live_sources &&
           !voice.queued && !device.callback;
}
bool fake_failures(const windows::ApprovedDeliveryWave &wave)
{
    for (int mode = 0; mode < 7; ++mode)
    {
        test::FakeXAudio2Api device;
        FakeVoiceApi voice{device, wave.view().data};
        if (mode == 0)
            device.engine_result = E_FAIL;
        if (mode == 1)
            device.mastering_result = E_FAIL;
        if (mode == 2 || mode == 3)
            voice.create_result = E_FAIL;
        voice.partial_source = mode == 3;
        voice.null_source = mode == 4;
        if (mode == 5)
            voice.submit_result = E_FAIL;
        if (mode == 6)
            voice.start_result = E_FAIL;
        {
            Playback player{wave, device, voice};
            if (player.play() != AudioPlayResult::unavailable ||
                player.available() || voice.queued || device.live_sources)
                return false;
            if (player.error() != (mode == 4 ? E_UNEXPECTED : E_FAIL))
                return false;
            const auto count =
                voice.creates + voice.submits + voice.starts + voice.destroys;
            if (player.play() != AudioPlayResult::unavailable ||
                voice.creates + voice.submits + voice.starts + voice.destroys !=
                    count)
                return false;
            device.engine_result = device.mastering_result = S_OK;
            voice.create_result = voice.submit_result = voice.start_result =
                S_OK;
            voice.partial_source = voice.null_source = false;
            if (player.recreate() != S_OK || !player.available() ||
                player.play() != AudioPlayResult::played)
                return false;
            voice.submit_result = E_OUTOFMEMORY;
            if (player.play() != AudioPlayResult::unavailable || voice.queued ||
                player.error() != E_OUTOFMEMORY)
                return false;
        }
        if (!voice.valid || !device.valid || device.live_sources ||
            device.callback)
            return false;
    }
    // A moved-from owner cannot be used as a fallback empty sound.
    auto donor = wave;
    auto keep = std::move(donor);
    test::FakeXAudio2Api device;
    FakeVoiceApi voice{device, keep.view().data};
    {
        Playback empty{std::move(donor), device, voice};
        if (empty.available() || empty.error() != E_INVALIDARG ||
            empty.play() != AudioPlayResult::unavailable || voice.creates)
            return false;
    }
    return device.valid && voice.valid;
}

class CapturingSystemApi final : public windows::XAudio2VoiceApi
{
  public:
    IXAudio2SourceVoice *source = nullptr;
    unsigned submissions = 0;
    bool first_submission_queued = false;
    windows::SystemXAudio2VoiceApi real;
    HRESULT create_source(IXAudio2 *engine, const WAVEFORMATEX &format,
                          IXAudio2SourceVoice **out) noexcept override
    {
        const auto result = real.create_source(engine, format, out);
        source = *out;
        return result;
    }
    HRESULT submit(IXAudio2SourceVoice *voice,
                   const XAUDIO2_BUFFER &buffer) noexcept override
    {
        const auto result = real.submit(voice, buffer);
        if (SUCCEEDED(result) && ++submissions == 1)
        {
            // The first source is still stopped, so this observation cannot
            // miss the buffer even if the test thread is later descheduled.
            XAUDIO2_VOICE_STATE state{};
            voice->GetState(&state, XAUDIO2_VOICE_NOSAMPLESPLAYED);
            first_submission_queued = state.BuffersQueued == 1;
        }
        return result;
    }
    HRESULT start(IXAudio2SourceVoice *voice) noexcept override
    {
        return real.start(voice);
    }
    void destroy_source(IXAudio2SourceVoice *voice) noexcept override
    {
        real.destroy_source(voice);
        source = nullptr;
    }
};
bool real_playback(const windows::ApprovedDeliveryWave &wave)
{
    CapturingSystemApi api;
    {
        Playback player{wave, windows::system_xaudio2_api(), api};
        std::cout << "Playback HRESULT=" << player.error()
                  << " available=" << player.available() << '\n';
        if (!player.available())
            return FAILED(player.error());
        if (player.play() != AudioPlayResult::played || !api.source ||
            !api.first_submission_queued)
            return false;
        const auto deadline =
            std::chrono::steady_clock::now() + std::chrono::seconds{3};
        XAUDIO2_VOICE_STATE state{};
        UINT64 peak = 0;
        do
        {
            api.source->GetState(&state);
            peak = std::max(peak, state.SamplesPlayed);
            // END_OF_STREAM resets SamplesPlayed. The previously observed
            // queue must drain without Stop/Flush/Destroy calls.
            if (!state.BuffersQueued)
                break;
            std::this_thread::sleep_for(std::chrono::milliseconds{10});
        } while (std::chrono::steady_clock::now() < deadline);
        std::cout << "Peak samples=" << peak
                  << " queued=" << state.BuffersQueued << '\n';
        if (state.BuffersQueued || !player.available())
            return false;
        // Destruction while another approved buffer is queued is safe too.
        if (player.play() != AudioPlayResult::played)
            return false;
    }
    return !api.source;
}
int wmain(int argc, wchar_t **argv)
{
    if (argc != 2)
        return 1;
    auto wave = windows::ApprovedDeliveryWave::load(argv[1]);
    if (!wave)
        return 2;
    if (!fake_lifecycle(*wave))
        return 3;
    if (!fake_failures(*wave))
        return 4;
    const auto apartment = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(apartment))
        return 5;
    const auto played = real_playback(*wave);
    CoUninitialize();
    return played ? 0 : 6;
}

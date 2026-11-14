#include "../support/xaudio2_voice_fake.hpp"
#include "platform/windows/xaudio2_output.hpp"
using namespace closed_circuit;

bool check(const std::filesystem::path &root,
           const windows::ApprovedDeliveryWave &wave, int mode)
{
    test::FakeXAudio2Api device;
    test::FakeXAudio2VoiceApi voice{device, wave.view().data};
    if (mode == 1)
        device.engine_result = E_FAIL;
    if (mode == 2)
        device.mastering_result = E_FAIL;
    if (mode == 3)
        voice.submit_result = E_FAIL;
    if (mode == 4)
        voice.start_result = E_FAIL;
    {
        windows::XAudio2Output output{mode == 5 ? root / L"missing" : root,
                                      device, voice};
        AudioOutput &port = output;
        if (mode == 6)
            device.callback->OnCriticalError(XAUDIO2_E_DEVICE_INVALIDATED);
        for (int raw = 1; raw < 256; ++raw)
            if (port.play(static_cast<AudioCue>(raw)) !=
                AudioPlayResult::unknown)
                return false;
        port.set_muted(true);
        if (!port.muted() ||
            port.play(AudioCue::delivery) != AudioPlayResult::muted ||
            voice.submits || voice.starts)
            return false;
        port.set_muted(false);
        const auto expected =
            mode == 0 ? AudioPlayResult::played : AudioPlayResult::unavailable;
        if (port.muted() || port.play(AudioCue::delivery) != expected)
            return false;
        const auto calls = voice.creates + voice.submits + voice.starts;
        if (port.play(AudioCue::delivery) != expected ||
            (mode != 0 &&
             voice.creates + voice.submits + voice.starts != calls))
            return false;
        if (mode == 0 && (voice.submits != 2 || voice.starts != 1))
            return false;
        if (mode == 5 && device.count != 0)
            return false;
    }
    return voice.valid && device.valid && !device.live_sources &&
           !device.callback;
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 1;
    const auto wave = windows::ApprovedDeliveryWave::load(argv[1]);
    if (!wave)
        return 2;
    for (int mode = 0; mode < 7; ++mode)
        if (!check(argv[1], *wave, mode))
            return 3 + mode;
    return 0;
}

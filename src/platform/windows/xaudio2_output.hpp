#pragma once
#include "application/audio_output.hpp"
#include "platform/windows/xaudio2_device.hpp"

namespace closed_circuit::windows
{
// Owns the device through the application port. No cue has an approved file
// yet, so even a working device cannot report played or create a source voice.
class XAudio2Output final : public AudioOutput
{
  public:
    explicit XAudio2Output(XAudio2Api &api = system_xaudio2_api()) noexcept
        : device_(api)
    {
    }
    [[nodiscard]] AudioPlayResult play(AudioCue cue) noexcept override
    {
        return is_audio_cue(cue) ? AudioPlayResult::unavailable
                                 : AudioPlayResult::unknown;
    }

  private:
    XAudio2Device device_;
};
} // namespace closed_circuit::windows

#include "application/audio_output.hpp"
#include <vector>
using namespace closed_circuit;
namespace
{
class RecordingOutput final : public AudioOutput
{
  public:
    AudioPlayResult play(AudioCue cue) override
    {
        cues.push_back(cue);
        if (!is_audio_cue(cue))
            return AudioPlayResult::unknown;
        return muted ? AudioPlayResult::muted : AudioPlayResult::played;
    }
    std::vector<AudioCue> cues;
    bool muted = false;
};
} // namespace
int main()
{
    SilentAudioOutput silent;
    static_assert(noexcept(silent.play(AudioCue::delivery)));
    AudioOutput &port = silent;
    for (int repeat = 0; repeat < 3; ++repeat)
        for (int raw = 0; raw < 256; ++raw)
        {
            const auto cue = static_cast<AudioCue>(raw);
            if (is_audio_cue(cue) != (raw == 0))
                return 1;
            const auto expected = raw == 0 ? AudioPlayResult::unavailable
                                           : AudioPlayResult::unknown;
            if (port.play(cue) != expected || silent.play(cue) != expected)
                return 2;
        }
    // Callers see results through the port only, in call order.
    RecordingOutput recording;
    AudioOutput &recorded = recording;
    const auto unknown = static_cast<AudioCue>(7);
    if (recorded.play(AudioCue::delivery) != AudioPlayResult::played ||
        recorded.play(unknown) != AudioPlayResult::unknown)
        return 3;
    recording.muted = true;
    if (recorded.play(AudioCue::delivery) != AudioPlayResult::muted ||
        recording.cues !=
            std::vector{AudioCue::delivery, unknown, AudioCue::delivery})
        return 4;
    return 0;
}

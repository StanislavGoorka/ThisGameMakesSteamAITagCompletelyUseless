#pragma once
#include "platform/windows/checkpoint_resume.hpp"
#include "platform/windows/run_window.hpp"

namespace closed_circuit::windows
{
// A resumed run opens directly at its saved sector start, with the cautious
// restored_checkpoint achievement origin. Otherwise the start screen shows the
// marked read error, if any. Creating the window never writes a checkpoint.
[[nodiscard]] inline RunWindowResult
create_resumed_window(NativeWindow &window, const CheckpointResume &resume,
                      RunSeed seed, const std::filesystem::path &root,
                      const AssetManifest &manifest, const TextCatalog &catalog,
                      AchievementNoticeSink *notices, CheckpointStore *store)
{
    auto made = RunWindow::create(
        window, resume.run ? *resume.run : start_run(seed), root, manifest, {},
        &catalog, !resume.run, notices, store);
    if (!made.session)
        return made;
    if (const auto status = made.session->show_resume_status(resume.status);
        FAILED(status))
        return {nullptr, status};
    return made;
}
} // namespace closed_circuit::windows

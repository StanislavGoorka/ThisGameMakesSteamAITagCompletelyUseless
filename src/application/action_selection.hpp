#pragma once
#include "application/keyboard_commands.hpp"
#include <utility>

namespace closed_circuit
{
// Input has already passed the held-key/focus filter. This model owns no run.
class ActionSelection
{
  public:
    [[nodiscard]] bool planning() const noexcept
    {
        return planning_;
    }
    [[nodiscard]] const std::optional<Command> &candidate() const noexcept
    {
        return candidate_;
    }
    [[nodiscard]] std::optional<NodeId> pointed() const noexcept
    {
        return pointed_;
    }
    void point(std::optional<NodeId> node) noexcept
    {
        pointed_ = node;
    }
    void toggle() noexcept
    {
        planning_ = !planning_;
        candidate_.reset();
    }
    void cancel() noexcept
    {
        planning_ = false;
        candidate_.reset();
    }
    void reset() noexcept
    {
        cancel();
        pointed_.reset();
    }
    // An external context change must discard any command from the old state.
    void context_changed() noexcept
    {
        candidate_.reset();
    }
    [[nodiscard]] std::optional<Command> choose(InputKey key, NodeId player)
    {
        auto command = map_key_command(key, player);
        if (key == InputKey::rotate)
            command = RotateCommand{pointed_.value_or(player)};
        else if (key == InputKey::overdrive)
            command = OverdriveCommand{pointed_.value_or(player)};
        if (!planning_)
            return command;
        candidate_ = std::move(command);
        return std::nullopt;
    }
    [[nodiscard]] std::optional<Command> confirm() noexcept
    {
        return planning_ ? std::exchange(candidate_, std::nullopt)
                         : std::nullopt;
    }

  private:
    bool planning_ = false;
    std::optional<Command> candidate_;
    std::optional<NodeId> pointed_;
};
} // namespace closed_circuit

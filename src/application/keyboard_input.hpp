#pragma once
#include <array>
#include <cstddef>

namespace closed_circuit
{
enum class InputKey : unsigned
{
    north,
    east,
    south,
    west,
    rotate,
    overdrive,
    wait,
    plan,
    confirm,
    cancel,
    count
};

class KeyboardInput
{
  public:
    void set_focus(bool focused) noexcept
    {
        focused_ = focused;
        if (!focused)
            held_.fill(false);
    }
    [[nodiscard]] bool key_down(InputKey key, bool repeated) noexcept
    {
        const auto index = static_cast<std::size_t>(key);
        if (!focused_ || index >= held_.size() || repeated || held_[index])
            return false;
        held_[index] = true;
        return true;
    }
    void key_up(InputKey key) noexcept
    {
        const auto index = static_cast<std::size_t>(key);
        if (index < held_.size())
            held_[index] = false;
    }

  private:
    bool focused_ = false;
    std::array<bool, static_cast<std::size_t>(InputKey::count)> held_{};
};
} // namespace closed_circuit

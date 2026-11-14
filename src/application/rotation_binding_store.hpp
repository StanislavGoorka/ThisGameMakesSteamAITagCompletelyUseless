#pragma once
#include "application/rotation_binding.hpp"

namespace closed_circuit
{
class RotationBindingStore
{
  public:
    virtual ~RotationBindingStore() = default;
    [[nodiscard]] virtual bool save(RotationBinding binding) = 0;
};
} // namespace closed_circuit

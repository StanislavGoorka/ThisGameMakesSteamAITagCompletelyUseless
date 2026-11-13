#pragma once
#include "application/text_catalog.hpp"

#ifndef CLOSED_CIRCUIT_PUBLIC_BUILD
#error Build must explicitly select CLOSED_CIRCUIT_PUBLIC_BUILD
#endif
namespace closed_circuit
{
inline constexpr TextAudience build_text_audience =
    CLOSED_CIRCUIT_PUBLIC_BUILD ? TextAudience::public_release
                                : TextAudience::developer;
} // namespace closed_circuit

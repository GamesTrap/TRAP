#ifndef TRAP_UTILS_BIT_H
#define TRAP_UTILS_BIT_H

#include <concepts>

/// @brief Shift 1 x times.
/// @param x Amount to shift.
/// @return Shifted value.
[[nodiscard]] constexpr std::unsigned_integral auto BIT(const std::unsigned_integral auto x) noexcept
{
	return decltype(x)(1) << x;
}

#endif /*TRAP_UTILS_BIT_H*/

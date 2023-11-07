#ifndef CONVERTTOSPIRV_VERSION_H
#define CONVERTTOSPIRV_VERSION_H

#include <cstdint>

#include "Types.h"

/// @brief Construct a version number.
/// @param major Major version number.
/// @param minor Minor version number.
/// @param patch Patch version number.
/// @return Version number packed into a single u32.
[[nodiscard]] constexpr u32 CONVERTTOSPIRV_MAKE_VERSION(const u32 major, const u32 minor, const u32 patch) noexcept
{
	return major << 22u | minor << 12u | patch;
}

//-------------------------------------------------------------------------------------------------------------------//

/// @brief Extract major version from a version number created with CONVERTTOSPIRV_MAKE_VERSION.
/// @param version Version number created with CONVERTTOSPIRV_MAKE_VERSION.
/// @return Major version number.
[[nodiscard]] constexpr u32 CONVERTTOSPIRV_VERSION_MAJOR(const u32 version) noexcept
{
	return version >> 22u;
}

//-------------------------------------------------------------------------------------------------------------------//

/// @brief Extract minor version from a version number created with CONVERTTOSPIRV_MAKE_VERSION.
/// @param version Version number created with CONVERTTOSPIRV_MAKE_VERSION.
/// @return Minor version number.
[[nodiscard]] constexpr u32 CONVERTTOSPIRV_VERSION_MINOR(const u32 version) noexcept
{
	return version >> 12u;
}

//-------------------------------------------------------------------------------------------------------------------//

/// @brief Extract patch version from a version number created with CONVERTTOSPIRV_MAKE_VERSION.
/// @param version Version number created with CONVERTTOSPIRV_MAKE_VERSION.
/// @return Patch version number.
[[nodiscard]] constexpr u32 CONVERTTOSPIRV_VERSION_PATCH(const u32 version) noexcept
{
	return version & 0xFFFu;
}

//-------------------------------------------------------------------------------------------------------------------//

/// @brief ConvertToSPIRV version number created with CONVERTTOSPIRV_MAKE_VERSION
inline constexpr u32 CONVERTTOSPIRV_VERSION = CONVERTTOSPIRV_MAKE_VERSION(0, 3, 0);

#endif /*CONVERTTOSPIRV_VERSION_H*/
#ifndef CONVERTTOSPIRV_VERSION_H
#define CONVERTTOSPIRV_VERSION_H

#include <cstdint>

#include "Types.h"

/// <summary>
/// Construct a version number.
/// </summary>
/// <param name="major">Major version number.</param>
/// <param name="minor">Minor version number.</param>
/// <param name="patch">Patch version number.</param>
/// <returns>Version number packed into a single u32.</returns>
[[nodiscard]] constexpr u32 CONVERTTOSPIRV_MAKE_VERSION(const u32 major, const u32 minor, const u32 patch) noexcept
{
	return major << 22u | minor << 12u | patch;
}

//-------------------------------------------------------------------------------------------------------------------//

/// <summary>
/// Extract major version from a version number created with CONVERTTOSPIRV_MAKE_VERSION.
/// </summary>
/// <param name="version">Version number created with CONVERTTOSPIRV_MAKE_VERSION.</param>
/// <returns>Major version number.</returns>
[[nodiscard]] constexpr u32 CONVERTTOSPIRV_VERSION_MAJOR(const u32 version) noexcept
{
	return version >> 22u;
}

//-------------------------------------------------------------------------------------------------------------------//

/// <summary>
/// Extract minor version from a version number created with CONVERTTOSPIRV_MAKE_VERSION.
/// </summary>
/// <param name="version">Version number created with CONVERTTOSPIRV_MAKE_VERSION.</param>
/// <returns>Minor version number.</returns>
[[nodiscard]] constexpr u32 CONVERTTOSPIRV_VERSION_MINOR(const u32 version) noexcept
{
	return version >> 12u;
}

//-------------------------------------------------------------------------------------------------------------------//

/// <summary>
/// Extract patch version from a version number created with CONVERTTOSPIRV_MAKE_VERSION.
/// </summary>
/// <param name="version">Version number created with CONVERTTOSPIRV_MAKE_VERSION.</param>
/// <returns>Patch version number.</returns>
[[nodiscard]] constexpr u32 CONVERTTOSPIRV_VERSION_PATCH(const u32 version) noexcept
{
	return version & 0xFFFu;
}

//-------------------------------------------------------------------------------------------------------------------//

/// <summary>
/// ConvertToSPIRV version number created with CONVERTTOSPIRV_MAKE_VERSION
/// </summary>
inline constexpr u32 CONVERTTOSPIRV_VERSION = CONVERTTOSPIRV_MAKE_VERSION(0, 3, 0);

#endif /*CONVERTTOSPIRV_VERSION_H*/
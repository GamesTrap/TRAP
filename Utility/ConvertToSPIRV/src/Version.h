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
/// <returns>Version number packed into a single uint32_t.</returns>
[[nodiscard]] constexpr uint32_t CONVERTTOSPIRV_MAKE_VERSION(const uint32_t major, const uint32_t minor, const uint32_t patch) noexcept
{
	return major << 22u | minor << 12u | patch;
}

//-------------------------------------------------------------------------------------------------------------------//

/// <summary>
/// Extract major version from a version number created with CONVERTTOSPIRV_MAKE_VERSION.
/// </summary>
/// <param name="version">Version number created with CONVERTTOSPIRV_MAKE_VERSION.</param>
/// <returns>Major version number.</returns>
[[nodiscard]] constexpr uint32_t CONVERTTOSPIRV_VERSION_MAJOR(const uint32_t version) noexcept
{
	return version >> 22u;
}

//-------------------------------------------------------------------------------------------------------------------//

/// <summary>
/// Extract minor version from a version number created with CONVERTTOSPIRV_MAKE_VERSION.
/// </summary>
/// <param name="version">Version number created with CONVERTTOSPIRV_MAKE_VERSION.</param>
/// <returns>Minor version number.</returns>
[[nodiscard]] constexpr uint32_t CONVERTTOSPIRV_VERSION_MINOR(const uint32_t version) noexcept
{
	return version >> 12u;
}

//-------------------------------------------------------------------------------------------------------------------//

/// <summary>
/// Extract patch version from a version number created with CONVERTTOSPIRV_MAKE_VERSION.
/// </summary>
/// <param name="version">Version number created with CONVERTTOSPIRV_MAKE_VERSION.</param>
/// <returns>Patch version number.</returns>
[[nodiscard]] constexpr uint32_t CONVERTTOSPIRV_VERSION_PATCH(const uint32_t version) noexcept
{
	return version & 0xFFFu;
}

//-------------------------------------------------------------------------------------------------------------------//

/// <summary>
/// ConvertToSPIRV version number created with CONVERTTOSPIRV_MAKE_VERSION
/// </summary>
inline constexpr uint32_t CONVERTTOSPIRV_VERSION = CONVERTTOSPIRV_MAKE_VERSION(0, 3, 0);

#endif /*CONVERTTOSPIRV_VERSION_H*/
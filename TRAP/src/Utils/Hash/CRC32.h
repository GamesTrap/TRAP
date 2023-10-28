#ifndef TRAP_CRC32_H
#define TRAP_CRC32_H

#include <array>
#include <string_view>

#include "Core/Types.h"

namespace TRAP::Utils::Hash
{
	/// <summary>
	/// Retrieve the CRC32 hash of the given data.
	/// </summary>
	/// <param name="data">Data to get hash from.</param>
	/// <param name="length">Length of the data in bytes.</param>
	/// <returns>CRC32 hash of input.</returns>
	[[nodiscard]] std::array<u8, 4> CRC32(const void* data, u64 length);
	/// <summary>
	/// Retrieve the CRC32 hash of the given string.
	/// </summary>
	/// <param name="str">String to get hash from.</param>
	/// <returns>CRC32 hash of input.</returns>
	[[nodiscard]] std::array<u8, 4> CRC32(std::string_view str);
}

#endif /*TRAP_CRC32_H*/
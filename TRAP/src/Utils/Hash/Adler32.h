#ifndef TRAP_ADLER32_H
#define TRAP_ADLER32_H

#include <array>
#include <string_view>

#include "Core/Types.h"

namespace TRAP::Utils::Hash
{
	/// <summary>
	/// Retrieve the Adler32 checksum of the given data.
	/// </summary>
	/// <param name="data">Data to get checksum from.</param>
	/// <param name="length">Length of the data in bytes.</param>
	/// <returns>Adler32 checksum of input.</returns>
	[[nodiscard]] std::array<u8, 4> Adler32(const void* data, u64 length);
	/// <summary>
	/// Retrieve the Adler32 checksum of the given string.
	/// </summary>
	/// <param name="str">String to get checksum from.</param>
	/// <returns>Adler32 checksum of input.</returns>
	[[nodiscard]] std::array<u8, 4> Adler32(std::string_view str);
}

#endif /*TRAP_ADLER32_H*/
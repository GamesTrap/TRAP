#ifndef TRAP_ADLER32_H
#define TRAP_ADLER32_H

#include <string_view>
#include <array>

#include "Core/Types.h"

namespace TRAP::Utils::Hash
{
	/// @brief Retrieve the Adler32 checksum of the given data.
	/// @param data Data to get checksum from.
	/// @param length Length of the data in bytes.
	/// @return Adler32 checksum of input.
	[[nodiscard]] std::array<u8, 4u> Adler32(const void* data, u64 length);
	/// @brief Retrieve the Adler32 checksum of the given string.
	/// @param str String to get checksum from.
	/// @return Adler32 checksum of input.
	[[nodiscard]] std::array<u8, 4u> Adler32(std::string_view str);
}

#endif /*TRAP_ADLER32_H*/

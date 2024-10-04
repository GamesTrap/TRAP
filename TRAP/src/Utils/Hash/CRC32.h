#ifndef TRAP_CRC32_H
#define TRAP_CRC32_H

#include <string_view>

#include "Core/Types.h"

namespace TRAP::Utils::Hash
{
	/// @brief Retrieve the CRC32 hash of the given data.
	/// @param data Data to get hash from.
	/// @param length Length of the data in bytes.
	/// @return CRC32 hash of input.
	[[nodiscard]] std::array<u8, 4u> CRC32(const void* data, u64 length);
	/// @brief Retrieve the CRC32 hash of the given string.
	/// @param str String to get hash from.
	/// @return CRC32 hash of input.
	[[nodiscard]] std::array<u8, 4u> CRC32(std::string_view str);
}

#endif /*TRAP_CRC32_H*/

#ifndef TRAP_SHA2_H
#define TRAP_SHA2_H

#include <array>
#include <string_view>

#include "Core/Types.h"

namespace TRAP::Utils::Hash
{
	/// @brief Retrieve the SHA2 256 hash of the given data.
	/// @param data Data to get hash from.
	/// @param length Length of the data in bytes.
	/// @return SHA2 256 hash of input.
	[[nodiscard]] std::array<u8, 32u> SHA2_256(const void* data, u64 length);
	/// @brief Retrieve the SHA2 256 hash of the given string.
	/// @param str String to get hash from.
	/// @return SHA2 256 hash of input.
	[[nodiscard]] std::array<u8, 32u> SHA2_256(std::string_view str);

	/// @brief Retrieve the SHA2 512 hash of the given data.
	/// @param data Data to get hash from.
	/// @param length Length of the data in bytes.
	/// @return SHA2 512 hash of input.
	[[nodiscard]] std::array<u8, 64u> SHA2_512(const void* data, u64 length);
	/// @brief Retrieve the SHA2 512 hash of the given string.
	/// @param str String to get hash from.
	/// @return SHA2 512 hash of input.
	[[nodiscard]] std::array<u8, 64u> SHA2_512(std::string_view str);
}

#endif /*TRAP_SHA2_H*/

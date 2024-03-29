#ifndef TRAP_SHA3_H
#define TRAP_SHA3_H

#include <array>
#include <string_view>

#include "Core/Types.h"

namespace TRAP::Utils::Hash
{
	/// <summary>
	/// Retrieve the SHA3 256 hash of the given data.
	/// </summary>
	/// <param name="data">Data to get hash from.</param>
	/// <param name="length">Length of the data in bytes.</param>
	/// <returns>SHA3 256 hash of input.</returns>
	[[nodiscard]] std::array<u8, 32> SHA3_256(const void* data, u64 length);
	/// <summary>
	/// Retrieve the SHA3 256 hash of the given string.
	/// </summary>
	/// <param name="str">String to get hash from.</param>
	/// <returns>SHA3 256 hash of input.</returns>
	[[nodiscard]] std::array<u8, 32> SHA3_256(std::string_view str);

	/// <summary>
	/// Retrieve the SHA3 512 hash of the given data.
	/// </summary>
	/// <param name="data">Data to get hash from.</param>
	/// <param name="length">Length of the data in bytes.</param>
	/// <returns>SHA3 512 hash of input.</returns>
	[[nodiscard]] std::array<u8, 64> SHA3_512(const void* data, u64 length);
	/// <summary>
	/// Retrieve the SHA3 512 hash of the given string.
	/// </summary>
	/// <param name="str">String to get hash from.</param>
	/// <returns>SHA3 512 hash of input.</returns>
	[[nodiscard]] std::array<u8, 64> SHA3_512(std::string_view str);
}

#endif /*TRAP_SHA3_H*/
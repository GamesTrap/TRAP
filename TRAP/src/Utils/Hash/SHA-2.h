#ifndef TRAP_SHA2_H
#define TRAP_SHA2_H

#include <array>
#include <cstdint>
#include <string_view>

namespace TRAP::Utils::Hash
{
	/// <summary>
	/// Retrieve the SHA2 256 hash of the given data.
	/// </summary>
	/// <param name="data">Data to get hash from.</param>
	/// <param name="length">Length of the data in bytes.</param>
	/// <returns>SHA2 256 hash of input.</returns>
	std::array<uint8_t, 32> SHA2_256(const void* data, uint64_t length);
	/// <summary>
	/// Retrieve the SHA2 256 hash of the given string.
	/// </summary>
	/// <param name="str">String to get hash from.</param>
	/// <returns>SHA2 256 hash of input.</returns>
	std::array<uint8_t, 32> SHA2_256(std::string_view str);

	/// <summary>
	/// Retrieve the SHA2 512 hash of the given data.
	/// </summary>
	/// <param name="data">Data to get hash from.</param>
	/// <param name="length">Length of the data in bytes.</param>
	/// <returns>SHA2 512 hash of input.</returns>
	std::array<uint8_t, 64> SHA2_512(const void* data, uint64_t length);
	/// <summary>
	/// Retrieve the SHA2 512 hash of the given string.
	/// </summary>
	/// <param name="str">String to get hash from.</param>
	/// <returns>SHA2 512 hash of input.</returns>
	std::array<uint8_t, 64> SHA2_512(std::string_view str);
}

#endif /*TRAP_SHA2_H*/
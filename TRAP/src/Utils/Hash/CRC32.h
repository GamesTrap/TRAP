#ifndef _TRAP_CRC32_H_
#define _TRAP_CRC32_H_

namespace TRAP::Utils::Hash
{
	/// <summary>
	/// Retrieve the CRC32 hash of the given data.
	/// </summary>
	/// <param name="data">Data to get hash from.</param>
	/// <param name="length">Length of the data in bytes.</param>
	/// <returns>CRC32 hash of input.</returns>
	std::array<uint8_t, 4> CRC32(const void* data, uint64_t length);
	/// <summary>
	/// Retrieve the CRC32 hash of the given string.
	/// </summary>
	/// <param name="str">String to get hash from.</param>
	/// <returns>CRC32 hash of input.</returns>
	std::array<uint8_t, 4> CRC32(std::string_view str);
}

#endif /*_TRAP_CRC32_H_*/
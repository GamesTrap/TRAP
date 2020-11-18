#ifndef _TRAP_ADLER32_H_
#define _TRAP_ADLER32_H_

namespace TRAP::Utils::Hash
{
	/// <summary>
	/// Retrieve the Adler32 checksum of the given data.
	/// </summary>
	/// <param name="data">Data to get checksum from.</param>
	/// <param name="length">Length of the data in bytes.</param>
	/// <returns>Adler32 checksum of input.</returns>
	std::array<uint8_t, 4> Adler32(const void* data, uint64_t length);
	/// <summary>
	/// Retrieve the Adler32 checksum of the given string.
	/// </summary>
	/// <param name="str">String to get checksum from.</param>
	/// <returns>Adler32 checksum of input.</returns>
	std::array<uint8_t, 4> Adler32(std::string_view str);
}

#endif /*_TRAP_ADLER32_H_*/
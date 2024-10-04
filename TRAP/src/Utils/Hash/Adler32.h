#ifndef TRAP_ADLER32_H
#define TRAP_ADLER32_H

#include <array>
#include <string_view>

#include "Core/Types.h"
#include "Utils/NumericCasts.h"

namespace TRAP::Utils::Hash
{
	/// @brief Retrieve the Adler32 checksum of the given data.
	/// @param data Data to get checksum from.
	/// @param length Length of the data in bytes.
	/// @return Adler32 checksum of input.
	template<typename T>
	requires std::same_as<T, char> || std::same_as<T, u8>
	[[nodiscard]] constexpr std::array<u8, 4u> Adler32(const T* data, u64 length);
	/// @brief Retrieve the Adler32 checksum of the given string.
	/// @param str String to get checksum from.
	/// @return Adler32 checksum of input.
	[[nodiscard]] constexpr std::array<u8, 4u> Adler32(std::string_view str);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
requires std::same_as<T, char> || std::same_as<T, u8>
[[nodiscard]] constexpr std::array<u8, 4u> TRAP::Utils::Hash::Adler32(const T* data, u64 length)
{
	u32 s1 = 1u & 0xFFFFu;
	u32 s2 = (1u >> 16u) & 0xFFFFu;

	while (length != 0u)
	{
		//At least 5552 sums can be done before the sums overflow, saving a lot of module divisions
		const u64 amount = length > 5552u ? 5552u : length;
		length -= amount;
		for (u64 i = 0u; i != amount; ++i)
		{
			s1 += static_cast<u8>(*data++);
			s2 += s1;
		}
		s1 %= 65521u;
		s2 %= 65521u;
	}

	const u32 adler32 = (s2 << 16u) | s1;

	return
	{
		NumericCast<u8>((adler32 >> 24u) & 0xFFu),
        NumericCast<u8>((adler32 >> 16u) & 0xFFu),
        NumericCast<u8>((adler32 >> 8u) & 0xFFu),
        NumericCast<u8>(adler32 & 0xFFu)
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::array<u8, 4u> TRAP::Utils::Hash::Adler32(const std::string_view str)
{
	return Adler32(str.data(), str.size());
}

#endif /*TRAP_ADLER32_H*/

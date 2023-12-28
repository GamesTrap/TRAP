#ifndef TRAP_CONVERTHASHTOSTRING_H
#define TRAP_CONVERTHASHTOSTRING_H

#include <array>
#include <string>

#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif /*_MSC_VER*/
//Tracy - Profiler
#include <tracy/Tracy.hpp>
#ifdef _MSC_VER
	#pragma warning(pop)
#endif /*_MSC_VER*/

#include "Core/Types.h"

namespace TRAP::Utils::Hash
{
	/// @brief Convert a hash(fixed-size array of bytes) into a string representation.
	/// @param hash N-sized array of bytes.
	/// @return String representation of given hash.
	template<usize N>
	[[nodiscard]] constexpr std::string ConvertHashToString(const std::array<u8, N>& hash)
	{
		constexpr std::array<char, 16> hexDigits{'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
		std::string result(hash.size() * 2, '\0');

		for(usize i = 0; i < hash.size(); ++i)
		{
			result[i * 2] = hexDigits[hash[i] >> 4u];
			result[i * 2 + 1] = hexDigits[hash[i] & 0xFu];
		}

		return result;
	}
}

#endif /*TRAP_CONVERTHASHTOSTRING_H*/
#ifndef TRAP_CONVERTHASHTOSTRING_H
#define TRAP_CONVERTHASHTOSTRING_H

#include "Core/Base.h"

#include <array>
#include <cstdint>
#include <iomanip>
#include <string>
#include <sstream>

#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif /*_MSC_VER*/
//Tracy - Profiler
#include <tracy/Tracy.hpp>
#ifdef _MSC_VER
	#pragma warning(pop)
#endif /*_MSC_VER*/

namespace TRAP::Utils::Hash
{
	/// <summary>
	/// Convert a hash(fixed-size array of bytes) into a string representation.
	/// </summary>
	/// <param name="hash">N-sized array of bytes.</param>
	/// <returns>String representation of given hash.</returns>
	template<std::size_t N>
	[[nodiscard]] constexpr std::string ConvertHashToString(const std::array<uint8_t, N>& hash)
	{
    	ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);

		constexpr std::array<char, 16> hexDigits{'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
		std::string result(hash.size() * 2, '\0');

		for(std::size_t i = 0; i < hash.size(); ++i)
		{
			result[i * 2] = hexDigits[hash[i] >> 4u];
			result[i * 2 + 1] = hexDigits[hash[i] & 0xF];
		}

		return result;
	}
}

#endif /*TRAP_CONVERTHASHTOSTRING_H*/
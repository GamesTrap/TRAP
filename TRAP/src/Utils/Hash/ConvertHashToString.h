#ifndef TRAP_CONVERTHASHTOSTRING_H
#define TRAP_CONVERTHASHTOSTRING_H

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
	[[nodiscard]] std::string ConvertHashToString(const std::array<uint8_t, N>& hash)
	{
    	ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);

		std::stringstream ss{};
		for (const uint8_t& i : hash)
		{
			ss << std::hex << std::setfill('0') << std::setw(2) << std::nouppercase
			   << static_cast<int32_t>(i);
		}

		return ss.str();
	}
}

#endif /*TRAP_CONVERTHASHTOSTRING_H*/
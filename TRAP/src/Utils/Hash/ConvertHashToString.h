#ifndef TRAP_CONVERTHASHTOSTRING_H
#define TRAP_CONVERTHASHTOSTRING_H

#include <array>
#include <cstdint>
#include <iomanip>
#include <string>
#include <sstream>

#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif
//Tracy - Profiler
#include <Tracy.hpp>
#ifdef _MSC_VER
	#pragma warning(pop)
#endif

namespace TRAP::Utils::Hash
{
	/// <summary>
	/// Convert a hash(fixed-size array of bytes) into a string representation.
	/// </summary>
	/// <param name="hash">N-sized array of bytes.</param>
	/// <returns>String representation of given hash.</returns>
	template<std::size_t N>
	std::string ConvertHashToString(const std::array<uint8_t, N>& hash)
	{
    	ZoneScoped;

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
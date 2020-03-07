#ifndef _TRAP_CONVERTHASHTOSTRING_H_
#define _TRAP_CONVERTHASHTOSTRING_H_

#include "TRAPPCH.h"

namespace TRAP::Utils::Hash
{
	template<std::size_t N>
	std::string ConvertHashToString(const std::array<uint8_t, N>& hash)
	{
		std::stringstream ss{};
		for (const uint8_t& i : hash)
			ss << std::hex << std::setfill('0') << std::setw(2) << std::nouppercase << static_cast<int>(static_cast<uint8_t>(i));
		return ss.str();
	}
}

#endif /*_TRAP_CONVERTHASHTOSTRING_H_*/
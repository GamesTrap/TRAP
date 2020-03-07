#ifndef _TRAP_SHA3_H_
#define _TRAP_SHA3_H_

namespace TRAP::Utils::Hash
{
	std::array<uint8_t, 32> SHA3_256(const void* data, uint64_t length);
	std::array<uint8_t, 32> SHA3_256(const std::string& str);

	std::array<uint8_t, 64> SHA3_512(const void* data, uint64_t length);
	std::array<uint8_t, 64> SHA3_512(const std::string& str);
}

#endif /*_TRAP_SHA3_H_*/
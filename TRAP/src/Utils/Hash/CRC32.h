#ifndef _TRAP_CRC32_H_
#define _TRAP_CRC32_H_

namespace TRAP::Utils::Hash
{
	std::array<uint8_t, 4> CRC32(const void* data, uint64_t length);
	std::array<uint8_t, 4> CRC32(std::string_view str);
}

#endif /*_TRAP_CRC32_H_*/
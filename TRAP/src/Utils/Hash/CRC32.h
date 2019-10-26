#ifndef _TRAP_CRC32_H_
#define _TRAP_CRC32_H_

namespace TRAP::Utils::Hash
{
	uint32_t CRC32(const void* data, std::size_t length, uint32_t previousCRC32 = 0);
}

#endif /*_TRAP_CRC32_H_*/
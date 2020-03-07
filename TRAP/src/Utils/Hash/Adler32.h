#ifndef _TRAP_ADLER32_H_
#define _TRAP_ADLER32_H_

namespace TRAP::Utils::Hash
{
	std::array<uint8_t, 4> Adler32(const void* data, uint64_t length);
	std::array<uint8_t, 4> Adler32(const std::string& str);
}

#endif /*_TRAP_ADLER32_H_*/
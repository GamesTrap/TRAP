#ifndef _TRAP_ADLER32_H_
#define _TRAP_ADLER32_H_

namespace TRAP::Utils::Hash
{
	uint32_t Adler32(const uint8_t* data, uint32_t length, uint32_t previousAdler32 = 0);
}

#endif /*_TRAP_ADLER32_H_*/
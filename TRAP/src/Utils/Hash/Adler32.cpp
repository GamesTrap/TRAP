#include "TRAPPCH.h"
#include "Adler32.h"

uint32_t TRAP::Utils::Hash::Adler32(const uint8_t* data, uint32_t length, const uint32_t previousAdler32)
{
	uint32_t s1 = 1u & 0xFFFFu;
	uint32_t s2 = (1u >> 16u) & 0xFFFFu;
	if(previousAdler32)
	{
		s1 = previousAdler32 & 0xFFFFu;
		s2 = (previousAdler32 >> 16u) & 0xFFFFu;
	}


	while(length != 0u)
	{
		//At least 5552 sums can be done before the sums overflow, saving a lot of module divisions
		const uint32_t amount = length > 5552u ? 5552u : length;
		length -= amount;
		for(uint32_t i = 0; i != amount; ++i)
		{
			s1 += (*data++);
			s2 += s1;
		}
		s1 %= 65521u;
		s2 %= 65521u;
	}

	return (s2 << 16u) | s1;
}
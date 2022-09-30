#include "TRAPPCH.h"
#include "Adler32.h"

#include "Utils/Memory.h"

std::array<uint8_t, 4> TRAP::Utils::Hash::Adler32(const void* const data, uint64_t length)
{
	const uint8_t* dataPtr = static_cast<const uint8_t*>(data);

	uint32_t s1 = 1u & 0xFFFFu;
	uint32_t s2 = (1u >> 16u) & 0xFFFFu;

	while (length != 0u)
	{
		//At least 5552 sums can be done before the sums overflow, saving a lot of module divisions
		const uint64_t amount = length > 5552u ? 5552u : length;
		length -= amount;
		for (uint64_t i = 0; i != amount; ++i)
		{
			s1 += (*dataPtr++);
			s2 += s1;
		}
		s1 %= 65521u;
		s2 %= 65521u;
	}

	uint32_t adler32 = (s2 << 16u) | s1;
	Memory::SwapBytes(adler32);

	std::array<uint8_t, 4> result{};
	std::copy_n(reinterpret_cast<const uint8_t*>(&adler32), result.size(), result.data());

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

std::array<uint8_t, 4> TRAP::Utils::Hash::Adler32(const std::string_view str)
{
	return Adler32(str.data(), str.length());
}
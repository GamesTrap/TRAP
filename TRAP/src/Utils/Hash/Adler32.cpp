#include "TRAPPCH.h"
#include "Adler32.h"

#include "Utils/Memory.h"

[[nodiscard]] std::array<u8, 4> TRAP::Utils::Hash::Adler32(const void* const data, u64 length)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);

	const u8* dataPtr = static_cast<const u8*>(data);

	u32 s1 = 1u & 0xFFFFu;
	u32 s2 = (1u >> 16u) & 0xFFFFu;

	while (length != 0u)
	{
		//At least 5552 sums can be done before the sums overflow, saving a lot of module divisions
		const u64 amount = length > 5552u ? 5552u : length;
		length -= amount;
		for (u64 i = 0; i != amount; ++i)
		{
			s1 += (*dataPtr++);
			s2 += s1;
		}
		s1 %= 65521u;
		s2 %= 65521u;
	}

	u32 adler32 = (s2 << 16u) | s1;
	Memory::SwapBytes(adler32);

	std::array<u8, 4> result{};
	std::copy_n(reinterpret_cast<const u8*>(&adler32), result.size(), result.data());

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::array<u8, 4> TRAP::Utils::Hash::Adler32(const std::string_view str)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return Adler32(str.data(), str.length());
}
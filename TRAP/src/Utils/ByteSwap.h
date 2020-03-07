#ifndef _TRAP_BYTESWAP_H_
#define _TRAP_BYTESWAP_H_

namespace TRAP::Utils::Memory
{
	template<typename T>
	static constexpr void SwapBytes(T& t);
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
constexpr void TRAP::Utils::Memory::SwapBytes(T& t)
{
	TRAP_CORE_ASSERT(!true, "Invalid template type used for byte swapping!");
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
constexpr void TRAP::Utils::Memory::SwapBytes<float>(float& t)
{
	t = static_cast<float>(((static_cast<uint32_t>(t) & 0xFF000000) >> 24) | ((static_cast<uint32_t>(t) & 0x00FF0000) >> 8) |
		((static_cast<uint32_t>(t) & 0x0000FF00) << 8) | ((static_cast<uint32_t>(t) & 0x000000FF) << 24));
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
constexpr void TRAP::Utils::Memory::SwapBytes<uint16_t>(uint16_t& t)
{
	t = (((t & 0xFF00) >> 8) | ((t & 0x00FF) << 8));
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
constexpr void TRAP::Utils::Memory::SwapBytes<uint32_t>(uint32_t& t)
{
	t = (((t & 0xFF000000) >> 24) | ((t & 0x00FF0000) >> 8) |
		((t & 0x0000FF00) << 8) | ((t & 0x000000FF) << 24));
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
constexpr void TRAP::Utils::Memory::SwapBytes<int32_t>(int32_t& t)
{
	t = ((t << 8) & 0xFF00FF00) | ((t >> 8) & 0xFF00FF);
	t = (t << 16) | ((t >> 16) & 0xFFFF);
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
constexpr void TRAP::Utils::Memory::SwapBytes<uint64_t>(uint64_t& t)
{
	t = ((t << 8) & 0xFF00FF00FF00FF00ULL) | ((t >> 8) & 0x00FF00FF00FF00FFULL);
	t = ((t << 16) & 0xFFFF0000FFFF0000ULL) | ((t >> 16) & 0x0000FFFF0000FFFFULL);
	
	t = (t << 32) | (t >> 32);
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
constexpr void TRAP::Utils::Memory::SwapBytes<int64_t>(int64_t& t)
{
	t = ((t << 8) & 0xFF00FF00FF00FF00ULL) | ((t >> 8) & 0x00FF00FF00FF00FFULL);
	t = ((t << 16) & 0xFFFF0000FFFF0000ULL) | ((t >> 16) & 0x0000FFFF0000FFFFULL);
	
	t = (t << 32) | ((t >> 32) & 0xFFFFFFFFULL);
}

#endif /*_TRAP_BYTESWAP_H_*/
#ifndef _TRAP_BYTESWAP_H_
#define _TRAP_BYTESWAP_H_

namespace TRAP::Utils::Memory
{
	template<typename T>
	static void SwapBytes(T& t);
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
void TRAP::Utils::Memory::SwapBytes(T& t)
{
	TRAP_CORE_ASSERT(!true, "Invalid template type used for byte swapping!");
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline void TRAP::Utils::Memory::SwapBytes<float>(float& t)
{
	t = static_cast<float>(((static_cast<uint32_t>(t) & 0xFF000000) >> 24) | ((static_cast<uint32_t>(t) & 0x00FF0000) >> 8) |
		((static_cast<uint32_t>(t) & 0x0000FF00) << 8) | ((static_cast<uint32_t>(t) & 0x000000FF) << 24));
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline void TRAP::Utils::Memory::SwapBytes<uint16_t>(uint16_t& t)
{
	t = (((t & 0xFF00) >> 8) | ((t & 0x00FF) << 8));
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline void TRAP::Utils::Memory::SwapBytes<uint32_t>(uint32_t& t)
{
	t = (((t & 0xFF000000) >> 24) | ((t & 0x00FF0000) >> 8) |
		((t & 0x0000FF00) << 8) | ((t & 0x000000FF) << 24));
}

//-------------------------------------------------------------------------------------------------------------------//

//TODO I dont know if this function is working properly
template<>
inline void TRAP::Utils::Memory::SwapBytes<int32_t>(int32_t& t)
{
	t = (((t & 0xFF000000) >> 24) | ((t & 0x00FF0000) >> 8) |
		((t & 0x0000FF00) << 8) | ((t & 0x000000FF) << 24));
}

#endif /*_TRAP_BYTESWAP_H_*/
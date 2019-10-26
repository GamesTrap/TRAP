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
	uint8_t* ptr = reinterpret_cast<uint8_t*>(&t);
	uint8_t temp = ptr[0]; ptr[0] = ptr[3]; ptr[3] = temp;
	temp = ptr[1]; ptr[1] = ptr[2]; ptr[2] = temp;
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline void TRAP::Utils::Memory::SwapBytes<uint16_t>(uint16_t& t)
{
	uint8_t* ptr = reinterpret_cast<uint8_t*>(&t);
	const uint8_t temp = ptr[0];
	ptr[0] = ptr[1];
	ptr[1] = temp;
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline void TRAP::Utils::Memory::SwapBytes<uint32_t>(uint32_t& t)
{
	uint8_t* ptr = reinterpret_cast<uint8_t*>(&t);
	uint8_t temp = ptr[0]; ptr[0] = ptr[3]; ptr[3] = temp;
	temp = ptr[1]; ptr[1] = ptr[2]; ptr[2] = temp;
}

//-------------------------------------------------------------------------------------------------------------------//

//TODO I dont know if this function is working properly
template<>
inline void TRAP::Utils::Memory::SwapBytes<int32_t>(int32_t& t)
{
	uint8_t* ptr = reinterpret_cast<uint8_t*>(&t);
	uint8_t temp = ptr[0]; ptr[0] = ptr[3]; ptr[3] = temp;
	temp = ptr[1]; ptr[1] = ptr[2]; ptr[2] = temp;
}

#endif /*_TRAP_BYTESWAP_H_*/
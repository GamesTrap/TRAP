#ifndef TRAP_BYTESWAP_H
#define TRAP_BYTESWAP_H

#include <bitset>

namespace TRAP::Utils::Memory
{
	/// <summary>
	/// Converts primitive data types from big->little or little->big endian depending on
	/// given parameters endianness.
	/// </summary>
	/// <typeparam name="T">Primitive data type.</typeparam>
	/// <param name="t">Primitive data type.</param>
	template<typename T>
	inline static void SwapBytes(T& t)
	{
		TRAP_ASSERT(false, "Invalid template type used for byte swapping!");
	}
}

//-------------------------------------------------------------------------------------------------------------------//

/// <summary>
/// Converts primitive data types from big->little or little->big endian depending on given parameters endianness.
/// </summary>
/// <param name="t">Primitive data type.</param>
template<>
inline void TRAP::Utils::Memory::SwapBytes<float>(float& t)
{
	uint32_t ui32 = *reinterpret_cast<uint32_t*>(&t);

	ui32 = (((ui32 & 0xFF000000) >> 24) | ((ui32 & 0x00FF0000) >>  8) |
	        ((ui32 & 0x0000FF00) <<  8) | ((ui32 & 0x000000FF) << 24));

	t = *reinterpret_cast<float*>(&ui32);
}

//-------------------------------------------------------------------------------------------------------------------//

/// <summary>
/// Converts primitive data types from big->little or little->big endian depending on given parameters endianness.
/// </summary>
/// <param name="t">Primitive data type.</param>
template<>
inline void TRAP::Utils::Memory::SwapBytes<double>(double& t)
{
	uint64_t ui64 = *reinterpret_cast<uint64_t*>(&t);

	ui64 = ((ui64 <<  8) & 0xFF00FF00FF00FF00ULL) | ((ui64 >>  8) & 0x00FF00FF00FF00FFULL);
	ui64 = ((ui64 << 16) & 0xFFFF0000FFFF0000ULL) | ((ui64 >> 16) & 0x0000FFFF0000FFFFULL);

	ui64 = (ui64 << 32) | (ui64 >> 32);

	t = *reinterpret_cast<double*>(&ui64);
}

//-------------------------------------------------------------------------------------------------------------------//

/// <summary>
/// Converts primitive data types from big->little or little->big endian depending on given parameters endianness.
/// </summary>
/// <param name="t">Primitive data type.</param>
template<>
inline void TRAP::Utils::Memory::SwapBytes<uint16_t>(uint16_t& t)
{
	t = (static_cast<uint16_t>(((t & 0xFF00) >> 8)) | static_cast<uint16_t>(((t & 0x00FF) << 8)));
}

//-------------------------------------------------------------------------------------------------------------------//

/// <summary>
/// Converts primitive data types from big->little or little->big endian depending on given parameters endianness.
/// </summary>
/// <param name="t">Primitive data type.</param>
template<>
inline void TRAP::Utils::Memory::SwapBytes<int16_t>(int16_t& t)
{
	t = (static_cast<int16_t>(((t & 0xFF00) >> 8)) | static_cast<int16_t>(((t & 0x00FF) << 8)));
}

//-------------------------------------------------------------------------------------------------------------------//

/// <summary>
/// Converts primitive data types from big->little or little->big endian depending on given parameters endianness.
/// </summary>
/// <param name="t">Primitive data type.</param>
template<>
inline void TRAP::Utils::Memory::SwapBytes<uint32_t>(uint32_t& t)
{
	t = (((t & 0xFF000000) >> 24) | ((t & 0x00FF0000) >>  8) |
		 ((t & 0x0000FF00) <<  8) | ((t & 0x000000FF) << 24));
}

//-------------------------------------------------------------------------------------------------------------------//

/// <summary>
/// Converts primitive data types from big->little or little->big endian depending on given parameters endianness.
/// </summary>
/// <param name="t">Primitive data type.</param>
template<>
inline void TRAP::Utils::Memory::SwapBytes<int32_t>(int32_t& t)
{
	t = ((t << 8) & 0xFF00FF00) | ((t >> 8) & 0xFF00FF);
	t = (t << 16) | ((t >> 16) & 0xFFFF);
}

//-------------------------------------------------------------------------------------------------------------------//

/// <summary>
/// Converts primitive data types from big->little or little->big endian depending on given parameters endianness.
/// </summary>
/// <param name="t">Primitive data type.</param>
template<>
inline void TRAP::Utils::Memory::SwapBytes<uint64_t>(uint64_t& t)
{
	t = ((t <<  8) & 0xFF00FF00FF00FF00ULL) | ((t >>  8) & 0x00FF00FF00FF00FFULL);
	t = ((t << 16) & 0xFFFF0000FFFF0000ULL) | ((t >> 16) & 0x0000FFFF0000FFFFULL);

	t = (t << 32) | (t >> 32);
}

//-------------------------------------------------------------------------------------------------------------------//

/// <summary>
/// Converts primitive data types from big->little or little->big endian depending on given parameters endianness.
/// </summary>
/// <param name="t">Primitive data type.</param>
template<>
inline void TRAP::Utils::Memory::SwapBytes<int64_t>(int64_t& t)
{
	t = ((t <<  8) & 0xFF00FF00FF00FF00ULL) | ((t >>  8) & 0x00FF00FF00FF00FFULL);
	t = ((t << 16) & 0xFFFF0000FFFF0000ULL) | ((t >> 16) & 0x0000FFFF0000FFFFULL);

	t = (t << 32) | ((t >> 32) & 0xFFFFFFFFULL);
}

#endif /*TRAP_BYTESWAP_H*/
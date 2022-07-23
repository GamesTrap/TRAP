#ifndef TRAP_BYTESWAP_H
#define TRAP_BYTESWAP_H

#include <bitset>
#include <type_traits>

#include "Core/Base.h"
#include "TRAP_Assert.h"

namespace TRAP::Utils::Memory
{
	/// <summary>
	/// Converts primitive data types from big->little or little->big endian depending on
	/// given parameters endianness.
	/// </summary>
	/// <typeparam name="T">Primitive data type.</typeparam>
	/// <param name="t">Primitive data type.</param>
	template<typename T>
	inline static constexpr void SwapBytes(T& /*t*/)
	{
		static_assert(std::is_reference_v<T>, "T must be a reference type");
		static_assert(!std::is_const_v<std::remove_reference_t<T>>, "T must not be a const reference");
		static_assert(sizeof(T) == 0, "T must be a primitive type");
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
	float res;
    uint8_t* floatToConvert = reinterpret_cast<uint8_t*>(&t);
    uint8_t* returnFloat = reinterpret_cast<uint8_t*>(&res);

    returnFloat[0] = floatToConvert[3];
    returnFloat[1] = floatToConvert[2];
    returnFloat[2] = floatToConvert[1];
    returnFloat[3] = floatToConvert[0];

	t = res;
}

//-------------------------------------------------------------------------------------------------------------------//

/// <summary>
/// Converts primitive data types from big->little or little->big endian depending on given parameters endianness.
/// </summary>
/// <param name="t">Primitive data type.</param>
template<>
inline void TRAP::Utils::Memory::SwapBytes<double>(double& t)
{
	double res;
    uint8_t* floatToConvert = reinterpret_cast<uint8_t*>(&t);
    uint8_t* returnFloat = reinterpret_cast<uint8_t*>(&res);

    returnFloat[0] = floatToConvert[7];
    returnFloat[1] = floatToConvert[6];
    returnFloat[2] = floatToConvert[5];
    returnFloat[3] = floatToConvert[4];
    returnFloat[4] = floatToConvert[3];
    returnFloat[5] = floatToConvert[2];
    returnFloat[6] = floatToConvert[1];
    returnFloat[7] = floatToConvert[0];

	t = res;
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
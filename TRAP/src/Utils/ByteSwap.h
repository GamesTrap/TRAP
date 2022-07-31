#ifndef TRAP_BYTESWAP_H
#define TRAP_BYTESWAP_H

#include <bitset>
#include <type_traits>
#include <algorithm>

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
	inline static constexpr void SwapBytes(T& t)
	{
		// if constexpr(std::is_same_v<T, float>)
		// {
		// 	float res{};
		// 	uint8_t* floatToConvert = reinterpret_cast<uint8_t*>(&t);
		// 	uint8_t* returnFloat = reinterpret_cast<uint8_t*>(&res);

		// 	returnFloat[0] = floatToConvert[3];
		// 	returnFloat[1] = floatToConvert[2];
		// 	returnFloat[2] = floatToConvert[1];
		// 	returnFloat[3] = floatToConvert[0];

		// 	t = res;
		// }
		// else if constexpr(std::is_same_v<T, double>)
		// {
		// 	double res{};
		// 	uint8_t* floatToConvert = reinterpret_cast<uint8_t*>(&t);
		// 	uint8_t* returnFloat = reinterpret_cast<uint8_t*>(&res);

		// 	returnFloat[0] = floatToConvert[7];
		// 	returnFloat[1] = floatToConvert[6];
		// 	returnFloat[2] = floatToConvert[5];
		// 	returnFloat[3] = floatToConvert[4];
		// 	returnFloat[4] = floatToConvert[3];
		// 	returnFloat[5] = floatToConvert[2];
		// 	returnFloat[6] = floatToConvert[1];
		// 	returnFloat[7] = floatToConvert[0];

		// 	t = res;
		// }
		// else if constexpr(std::is_same_v<T, uint16_t> || std::is_same_v<T, int16_t>)
		// {
		// 	t = (static_cast<T>(((t & 0xFF00) >> 8)) | static_cast<T>(((t & 0x00FF) << 8)));
		// }
		// else if constexpr(std::is_same_v<T, uint32_t>)
		// {
		// 	t = (((t & 0xFF000000) >> 24) | ((t & 0x00FF0000) >>  8) |
		// 		 ((t & 0x0000FF00) <<  8) | ((t & 0x000000FF) << 24));
		// }
		// else if constexpr(std::is_same_v<T, int32_t>)
		// {
		// 	t = ((t << 8) & 0xFF00FF00) | ((t >> 8) & 0xFF00FF);
		// 	t = (t << 16) | ((t >> 16) & 0xFFFF);
		// }
		// else if constexpr(std::is_same_v<T, uint64_t>)
		// {
		// 	t = ((t <<  8) & 0xFF00FF00FF00FF00ULL) | ((t >>  8) & 0x00FF00FF00FF00FFULL);
		// 	t = ((t << 16) & 0xFFFF0000FFFF0000ULL) | ((t >> 16) & 0x0000FFFF0000FFFFULL);

		// 	t = (t << 32) | (t >> 32);
		// }
		// else if constexpr(std::is_same_v<T, int64_t>)
		// {
		// 	t = ((t <<  8) & 0xFF00FF00FF00FF00ULL) | ((t >>  8) & 0x00FF00FF00FF00FFULL);
		// 	t = ((t << 16) & 0xFFFF0000FFFF0000ULL) | ((t >> 16) & 0x0000FFFF0000FFFFULL);

		// 	t = (t << 32) | ((t >> 32) & 0xFFFFFFFFULL);
		// }
		if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>)
		{
		    uint8_t* ptr = reinterpret_cast<uint8_t*>(&t);
			std::array<std::uint8_t, sizeof(T)> raw_src, raw_dst;

			std::copy_n(ptr, sizeof(T), raw_src.data());
			std::reverse_copy(raw_src.begin(), raw_src.end(), raw_dst.begin());
			std::copy_n(raw_dst.data(), sizeof(T), ptr);
		}
		else
		{
			static_assert(sizeof(T) == 0, "T must be a primitive type");
		}
	}
}

#endif /*TRAP_BYTESWAP_H*/
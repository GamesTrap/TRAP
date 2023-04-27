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
	inline static void SwapBytes(T& t)
	{
		ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);

		if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>)
		{
		    uint8_t* const ptr = reinterpret_cast<uint8_t*>(&t);
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

	/// <summary>
	/// Converts all primitive data types in the container from
	/// big->little or little->big endian depending on
	/// given parameters endianness.
	/// </summary>
	/// <typeparam name="Iter">Container iterator type.</typeparam>
	/// <param name="begin">Start of the container.</param>
	/// <param name="end">End of the container.</param>
	template<typename Iter>
	inline static void SwapBytes(Iter begin, Iter end)
	{
		ZoneNamedC(__tracy, tracy::Color::Violet, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

		for(; begin != end; ++begin)
			SwapBytes(*begin);
	}

	/// <summary>
	/// Converts the given bytes to the given data type
	///
	/// Note: Byte order depends on the given input bytes.
	/// </summary>
	/// <typeparam name="T">Type to convert bytes to.</typeparam>
	/// <param name="source">Bytes to convert.</param>
	/// <returns>Converted bytes.</returns>
	template<typename T>
	[[nodiscard]] inline static T ConvertByte(const uint8_t* const source)
	{
		ZoneNamedC(__tracy, tracy::Color::Violet, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

		if constexpr(std::is_unsigned_v<T> && (std::is_same_v<T, uint16_t> ||
		             std::is_same_v<T, uint32_t> || std::is_same_v<T, uint64_t>))
		{
			if constexpr (sizeof(T) == 2)
			{
				return (static_cast<T>(source[0]) << 8u) | static_cast<T>(source[1]);
			}
			else if constexpr (sizeof(T) == 4)
			{
				return (static_cast<T>(source[0]) << 24u) | (static_cast<T>(source[1]) << 16u) |
				       (static_cast<T>(source[2]) << 8u) | static_cast<T>(source[3]);
			}
			else if constexpr (sizeof(T) == 8)
			{
				return (static_cast<T>(source[0]) << 56u) | (static_cast<T>(source[1]) << 48u) |
				       (static_cast<T>(source[2]) << 40u) | (static_cast<T>(source[3]) << 32u) |
				       (static_cast<T>(source[4]) << 24u) | (static_cast<T>(source[5]) << 16u) |
				       (static_cast<T>(source[6]) << 8u) | static_cast<T>(source[7]);
			}
		}
		else
		{
			static_assert(sizeof(T) == 0, "T must be unsigned interger type (not uint8_t");
			return {};
		}
	}

	/// <summary>
	/// Converts all bytes from begin to end iterators to the given type from
	/// the output iterator and stores the resulting data in output.
	///
	/// Note: Byte order depends on the given input bytes.
	/// Note: The output container must be big enough to hold the converted data.
	/// </summary>
	/// <typeparam name="InputIt">Container input iterator type.</typeparam>
	/// <typeparam name="OutputIt">Container output iterator type.</typeparam>
	/// <param name="begin">Start of the input container.</param>
	/// <param name="end">End of the input container.</param>
	/// <param name="output">Type to convert to and store the converted data.</param>
	template<typename InputIt, typename OutputIt>
	inline static void ConvertBytes(InputIt begin, InputIt end, OutputIt output)
	{
		ZoneNamedC(__tracy, tracy::Color::Violet, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

		using input_type = typename std::iterator_traits<InputIt>::value_type;
		using output_type = typename std::iterator_traits<OutputIt>::value_type;

		if constexpr(std::is_same_v<input_type, uint8_t>)
		{
			//Is size a multiple of OutputIts type
			const auto size = std::distance(begin, end);
			if(size % sizeof(output_type) != 0)
			{
				TRAP_ASSERT(false, "Memory::ConvertBytes(): Size of input type is not a multiple of output type");
				return;
			}

			for(; begin != end; begin += sizeof(output_type), ++output)
				*output = ConvertByte<output_type>(&(*begin));
		}
		else
		{
			static_assert(sizeof(InputIt) == 0, "InputIt must be a of type uint8_t");
		}
	}
}

#endif /*TRAP_BYTESWAP_H*/
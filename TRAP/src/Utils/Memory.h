#ifndef TRAP_BYTESWAP_H
#define TRAP_BYTESWAP_H

#include "Core/Base.h"
#include "TRAP_Assert.h"

namespace TRAP::Utils::Memory
{
	/// @brief Converts primitive data types from big->little or little->big endian depending on
	/// given parameters endianness.
	/// @param t Primitive data type.
	constexpr void SwapBytes(std::integral auto& t)
	{
		t = std::byteswap(t);
	}

	/// @brief Converts all primitive data types in the container from
	/// big->little or little->big endian depending on
	/// given parameters endianness.
	/// @tparam Iter Container iterator type.
	/// @param begin Start of the container.
	/// @param end End of the container.
	template<typename Iter>
	requires std::integral<typename std::iterator_traits<Iter>::value_type>
	constexpr void SwapBytes(Iter begin, Iter end)
	{
		for(; begin != end; ++begin)
			SwapBytes(*begin);
	}

	/// @brief Converts the given bytes to the given data type
	/// @tparam T Type to convert bytes to.
	/// @param source Bytes to convert.
	/// @return Converted bytes.
	/// @note Byte order depends on the given input bytes.
	template<typename T>
	requires (std::unsigned_integral<T> && !std::same_as<T, u8>)
	[[nodiscard]] constexpr T ConvertByte(const u8* const source)
	{
		if constexpr (sizeof(T) == 2)
		{
			return (static_cast<T>(source[0])) | (static_cast<T>(source[1]) << 8u);
		}
		else if constexpr (sizeof(T) == 4)
		{
			return (static_cast<T>(source[0])) | (static_cast<T>(source[1]) << 8u) |
					(static_cast<T>(source[2]) << 16u) | (static_cast<T>(source[3]) << 24u);
		}
		else if constexpr (sizeof(T) == 8)
		{
			return (static_cast<T>(source[0])) | (static_cast<T>(source[1]) << 8u) |
					(static_cast<T>(source[2]) << 16u) | (static_cast<T>(source[3]) << 24u) |
					(static_cast<T>(source[4]) << 32u) | (static_cast<T>(source[5]) << 40u) |
					(static_cast<T>(source[6]) << 48u) | (static_cast<T>(source[7]) << 56u);
		}
		else
		{
			static_assert(sizeof(T) == 0, "T has unknown byte size for unsigned interger type");
			return {};
		}
	}

	/// @brief Converts all bytes from begin to end iterators to the given type from
	/// the output iterator and stores the resulting data in output.
	/// @tparam InputIt Container input iterator type.
	/// @tparam OutputIt Container output iterator type.
	/// @param begin Start of the input container.
	/// @param end End of the input container.
	/// @param output Type to convert to and store the converted data.
	/// @note Byte order depends on the given input bytes.
	/// @note The output container must be big enough to hold the converted data.
	template<typename InputIt, typename OutputIt>
	requires std::same_as<typename std::iterator_traits<InputIt>::value_type, u8>
	static void ConvertBytes(InputIt begin, InputIt end, OutputIt output)
	{
		using output_type = typename std::iterator_traits<OutputIt>::value_type;

		//Is size a multiple of OutputIts type
		const auto size = std::distance(begin, end);
		if(NumericCast<u64>(size) % sizeof(output_type) != 0)
		{
			TRAP_ASSERT(false, "Memory::ConvertBytes(): Size of input type is not a multiple of output type");
			return;
		}

		for(; begin != end; begin += sizeof(output_type), ++output)
			*output = ConvertByte<output_type>(&(*begin));
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Retrieve a specific byte from the given integral.
	/// @tparam Byte Byte to retrieve.
	/// @return Requested byte from the given integral.
	template<u8 Byte>
	[[nodiscard]] constexpr u8 GetByteFromInteger(const std::integral auto value)
	requires (Byte < sizeof(decltype(value)))
	{
		return NumericCast<u8>((value >> (Byte * 8u)) & 0xFFu);
	}
}

#endif /*TRAP_BYTESWAP_H*/
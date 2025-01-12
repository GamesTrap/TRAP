#ifndef TRAP_MEMORY_H
#define TRAP_MEMORY_H

#include "Core/Backports/ByteSwap.h"
#include "TRAP_Assert.h"
#include "Utils/NumericCasts.h"

namespace TRAP::Utils::Memory::INTERNAL
{
	template<typename T>
	requires (std::unsigned_integral<T> && sizeof(T) > 1)
	[[nodiscard]] constexpr T ConvertByte(const u8* const source)
	{
		if constexpr (sizeof(T) == 2u)
		{
			return (static_cast<T>(source[0u])) | (static_cast<T>(source[1u]) << 8u);
		}
		else if constexpr (sizeof(T) == 4u)
		{
			return (static_cast<T>(source[0u])) | (static_cast<T>(source[1u]) << 8u) |
					(static_cast<T>(source[2u]) << 16u) | (static_cast<T>(source[3u]) << 24u);
		}
		else if constexpr (sizeof(T) == 8u)
		{
			return (static_cast<T>(source[0u])) | (static_cast<T>(source[1u]) << 8u) |
					(static_cast<T>(source[2u]) << 16u) | (static_cast<T>(source[3u]) << 24u) |
					(static_cast<T>(source[4u]) << 32u) | (static_cast<T>(source[5u]) << 40u) |
					(static_cast<T>(source[6u]) << 48u) | (static_cast<T>(source[7u]) << 56u);
		}
		else
		{
			static_assert(sizeof(T) == 0u, "T has unknown byte size for unsigned interger type");
			return {};
		}
	}
}

namespace TRAP::Utils::Memory
{
	/// @brief Converts primitive data types from big->little or little->big endian depending on
	/// given parameters endianness.
	/// @param t Primitive data type.
	constexpr void SwapBytes(std::integral auto& t)
	{
		t = std::byteswap(t);
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Converts all primitive data types in the container from
	/// big->little or little->big endian depending on
	/// given parameters endianness.
	/// @tparam Iter Container iterator type.
	/// @param begin Start of the container.
	/// @param end End of the container.
	template<typename Iter>
	requires std::integral<std::iter_value_t<Iter>>
	constexpr void SwapBytes(Iter begin, Iter end)
	{
		for(; begin != end; ++begin)
			SwapBytes(*begin);
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Converts the given bytes to the given data type
	/// @tparam T Type to convert bytes to.
	/// @param source Bytes to convert.
	/// @param size Length of source array, must be at least sizeof(T).
	/// @return Converted bytes on success, TRAP::NullOpt otherwise.
	/// @note Byte order depends on the given input bytes.
	template<typename T>
	requires (std::unsigned_integral<T> && sizeof(T) > 1)
	[[nodiscard]] constexpr TRAP::Optional<T> ConvertByte(const u8* const source, const usize size)
	{
		if(source == nullptr || size < sizeof(T))
		{
			TRAP_ASSERT(false);
			return TRAP::NullOpt;
		}

		return INTERNAL::ConvertByte<T>(source);
	}

	/// @brief Converts the given bytes to the given data type
	/// @tparam T Type to convert bytes to.
	/// @param source Bytes to convert where size() is at least sizeof(T).
	/// @return Converted bytes on success, TRAP::NullOpt otherwise.
	/// @note Byte order depends on the given input bytes.
	template<typename T, typename SpanType, usize Extent = std::dynamic_extent>
	requires (std::unsigned_integral<T> && sizeof(T) > 1) && std::same_as<std::remove_const_t<SpanType>, u8>
	[[nodiscard]] constexpr TRAP::Optional<T> ConvertByte(const std::span<SpanType, Extent> source)
	{
		if(source.size() < sizeof(T))
		{
			TRAP_ASSERT(false);
			return TRAP::NullOpt;
		}

		return INTERNAL::ConvertByte<T>(source.data());
	}

	//-------------------------------------------------------------------------------------------------------------------//

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
	requires std::same_as<std::iter_value_t<InputIt>, u8> &&
	         (std::unsigned_integral<std::iter_value_t<OutputIt>>) &&
			 (sizeof(std::iter_value_t<OutputIt>) > 1)
	constexpr void ConvertBytes(InputIt begin, InputIt end, OutputIt output)
	{
		using output_type = std::iter_value_t<OutputIt>;

		//Is size a multiple of OutputIts type
		const auto size = std::distance(begin, end);
		if(size == 0 || NumericCast<u64>(size) % sizeof(output_type) != 0u)
		{
			TRAP_ASSERT(false, "Memory::ConvertBytes(): Size of input type is not a multiple of output type");
			return;
		}

		for(; begin != end; begin += sizeof(output_type), ++output)
			*output = INTERNAL::ConvertByte<output_type>(&(*begin));
	}

	//-------------------------------------------------------------------------------------------------------------------//

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
	requires std::same_as<std::iter_value_t<InputIt>, u8> &&
	         (std::unsigned_integral<typename OutputIt::container_type::value_type>) &&
			 (sizeof(typename OutputIt::container_type::value_type) > 1)
	constexpr void ConvertBytes(InputIt begin, InputIt end, OutputIt output) //inserter overload
	{
		using output_type = OutputIt::container_type::value_type;

		//Is size a multiple of OutputIts type
		const auto size = std::distance(begin, end);
		if(size == 0 || NumericCast<u64>(size) % sizeof(output_type) != 0u)
		{
			TRAP_ASSERT(false, "Memory::ConvertBytes(): Size of input type is not a multiple of output type");
			return;
		}

		for(; begin != end; begin += sizeof(output_type), output)
			*output = INTERNAL::ConvertByte<output_type>(&(*begin));
	}
}

#endif /*TRAP_MEMORY_H*/

#ifndef TRAP_NUMERICCASTS_H
#define TRAP_NUMERICCASTS_H

#include <concepts>
#include <limits>
#include <string_view>

#include <fmt/format.h>

//Tracy - Profiler
#include <tracy/Tracy.hpp>

#include "Core/Backports.h"
#include "TRAP_Assert.h"

namespace INTERNAL
{
    template<typename X>
    requires std::is_arithmetic_v<X>
    constexpr std::string_view TypeNameToString()
    {
        if constexpr(std::same_as<X, i8>)
        {
            return "i8";
        }
        else if constexpr(std::same_as<X, u8>)
        {
            return "u8";
        }
        else if constexpr(std::same_as<X, i16>)
        {
            return "i16";
        }
        else if constexpr(std::same_as<X, u16>)
        {
            return "u16";
        }
        else if constexpr(std::same_as<X, i32>)
        {
            return "i32";
        }
        else if constexpr(std::same_as<X, u32>)
        {
            return "u32";
        }
        else if constexpr(std::same_as<X, i64>)
        {
            return "i64";
        }
        else if constexpr(std::same_as<X, u64>)
        {
            return "u64";
        }
        else if constexpr(std::same_as<X, f32>)
        {
            return "f32";
        }
        else if constexpr(std::same_as<X, f64>)
        {
            return "f64";
        }
        else
        {
            std::unreachable();
        }
    }

    template<typename To, typename From>
    concept NarrowFloatingPointToFloatingPoint = (std::floating_point<To> && std::floating_point<From> && (sizeof(To) <= sizeof(From)));

    template<typename To, typename From>
    concept NarrowFloatingPointToIntegral = (std::integral<To> && std::floating_point<From>);

    template<typename To, typename From>
    concept WidenSignedIntegralToUnsignedIntegral = (std::unsigned_integral<To> && std::signed_integral<From> && (sizeof(To) > sizeof(From)));

    template<typename To, typename From>
    concept NarrowIntegralToIntegral = (std::integral<To> && std::integral<From> && (std::is_signed_v<To> == std::is_signed_v<From>) && (sizeof(To) <= sizeof(From)));

    template<typename To, typename From>
    concept NarrowUnsignedIntegralToSignedIntegral = (std::signed_integral<To> && std::unsigned_integral<From> && (sizeof(To) <= sizeof(From)));

    template<typename To, typename From>
    concept NarrowSignedIntegralToUnsignedIntegral = (std::unsigned_integral<To> && std::signed_integral<From> && (sizeof(To) <= sizeof(From)));

    template<typename To, typename From>
    concept NarrowUnsignedIntegralToUnsignedIntegral = (std::unsigned_integral<To> && std::unsigned_integral<From> && (sizeof(To) <= sizeof(From)));
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename To, typename From>
requires std::is_arithmetic_v<To> && std::is_arithmetic_v<From>
[[nodiscard]] constexpr To NumericCast(const From value)
{
	static_assert(!std::is_enum_v<To> && !std::is_enum_v<From>, "NumericCast(): Casting to/from enum is not allowed!");
	static_assert(!std::is_scoped_enum_v<To> && !std::is_scoped_enum_v<From>, "NumericCast(): Casting to/from enum is not allowed!");
	static_assert(!std::same_as<To, From>, "NumericCast(): Casting to the same type is not allowed!");

#if !defined(TRAP_DEBUG) && !defined(TRAP_UNITTESTS)
    return static_cast<To>(value);
#else
    if constexpr(INTERNAL::NarrowFloatingPointToFloatingPoint<To, From> ||
                 INTERNAL::NarrowFloatingPointToIntegral<To, From> ||
                 INTERNAL::NarrowIntegralToIntegral<To, From>)
    {
        if(value > static_cast<From>(std::numeric_limits<To>::max()) ||
            value < static_cast<From>(std::numeric_limits<To>::lowest()))
        {
            TRAP_ASSERT(false, fmt::format("NumericCastError: Failed to cast {}({}) to type {}!", INTERNAL::TypeNameToString<From>(), value, INTERNAL::TypeNameToString<To>()));
        }

        return static_cast<To>(value);
    }
    else if constexpr(INTERNAL::WidenSignedIntegralToUnsignedIntegral<To, From>)
    {
        //Casting integral -> unsigned integral with increased byte size is considered unsafe
        if(value < static_cast<From>(0))
        {
            TRAP_ASSERT(false, fmt::format("NumericCastError: Failed to cast {}({}) to type {}!", INTERNAL::TypeNameToString<From>(), value, INTERNAL::TypeNameToString<To>()));
        }

        return static_cast<To>(value);
    }
    else if constexpr(INTERNAL::NarrowUnsignedIntegralToSignedIntegral<To, From>)
    {
        //Casting unsigned integral -> signed integral with decreased byte size is considered unsafe
        if(value > static_cast<From>(std::numeric_limits<To>::max()))
        {
            TRAP_ASSERT(false, fmt::format("NumericCastError: Failed to cast {}({}) to type {}!", INTERNAL::TypeNameToString<From>(), value, INTERNAL::TypeNameToString<To>()));
        }

        return static_cast<To>(value);
    }
    else if constexpr(INTERNAL::NarrowSignedIntegralToUnsignedIntegral<To, From>)
    {
        //Casting integral -> unsigned integral with decreased byte size is considered unsafe
        if(value < static_cast<From>(0) ||
            static_cast<To>(value) > static_cast<To>(std::numeric_limits<From>::max()))
        {
            TRAP_ASSERT(false, fmt::format("NumericCastError: Failed to cast {}({}) to type {}!", INTERNAL::TypeNameToString<From>(), value, INTERNAL::TypeNameToString<To>()));
        }

        return static_cast<To>(value);
    }
    else
    {
        return static_cast<To>(value);
    }
#endif
}

#endif /*TRAP_NUMERICCASTS_H*/

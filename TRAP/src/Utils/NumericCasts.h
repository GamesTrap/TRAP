#ifndef TRAP_NUMERICCASTS_H
#define TRAP_NUMERICCASTS_H

#include <concepts>
#include <limits>
#include <string>

#include <fmt/format.h>

//Tracy - Profiler
#include <tracy/Tracy.hpp>

#include "Core/Backports.h"
#include "TRAP_Assert.h"

namespace INTERNAL
{
    template<typename X>
    constexpr std::string_view TypeNameToString()
    {
        if constexpr(std::same_as<X, int8_t>)
        {
            return "int8_t";
        }
        else if constexpr(std::same_as<X, uint8_t>)
        {
            return "uint8_t";
        }
        else if constexpr(std::same_as<X, int16_t>)
        {
            return "int16_t";
        }
        else if constexpr(std::same_as<X, uint16_t>)
        {
            return "uint16_t";
        }
        else if constexpr(std::same_as<X, int32_t>)
        {
            return "int32_t";
        }
        else if constexpr(std::same_as<X, uint32_t>)
        {
            return "uint32_t";
        }
        else if constexpr(std::same_as<X, int64_t>)
        {
            return "int64_t";
        }
        else if constexpr(std::same_as<X, uint64_t>)
        {
            return "uint64_t";
        }
        else if constexpr(std::same_as<X, float>)
        {
            return "float";
        }
        else if constexpr(std::same_as<X, double>)
        {
            return "double";
        }

        std::unreachable();
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
	static_assert(!std::is_enum_v<To> && !std::is_enum_v<From>, "CheckedCast(): Casting to/from enum is not allowed!");
	static_assert(!std::is_scoped_enum_v<To> && !std::is_scoped_enum_v<From>, "CheckedCast(): Casting to/from enum is not allowed!");
	static_assert(!std::same_as<To, From>, "CheckedCast(): Casting to the same type is not allowed!");

#ifndef TRAP_DEBUG
    return static_cast<To>(value);
#else
    if constexpr(INTERNAL::NarrowFloatingPointToFloatingPoint<To, From> ||
                 INTERNAL::NarrowFloatingPointToIntegral<To, From> ||
                 INTERNAL::NarrowIntegralToIntegral<To, From>)
    {
        if(value > static_cast<From>(std::numeric_limits<To>::max()) ||
            value < static_cast<From>(std::numeric_limits<To>::lowest()))
        {
            TRAP_ASSERT(false, fmt::format("NumericCastError: Failed to cast {}() to type {}!", INTERNAL::TypeNameToString<From>(), value, INTERNAL::TypeNameToString<To>()));
        }

        return static_cast<To>(value);
    }
    else if constexpr(INTERNAL::WidenSignedIntegralToUnsignedIntegral<To, From>)
    {
        //Casting integral -> unsigned integral with increased byte size is considered unsafe
        if(value < static_cast<From>(0))
        {
            TRAP_ASSERT(false, fmt::format("NumericCastError: Failed to cast {}() to type {}!", INTERNAL::TypeNameToString<From>(), value, INTERNAL::TypeNameToString<To>()));
        }

        return static_cast<To>(value);
    }
    else if constexpr(INTERNAL::NarrowUnsignedIntegralToSignedIntegral<To, From>)
    {
        //Casting unsigned integral -> signed integral with decreased byte size is considered unsafe
        if(value > static_cast<From>(std::numeric_limits<To>::max()))
        {
            TRAP_ASSERT(false, fmt::format("NumericCastError: Failed to cast {}() to type {}!", INTERNAL::TypeNameToString<From>(), value, INTERNAL::TypeNameToString<To>()));
        }

        return static_cast<To>(value);
    }
    else if constexpr(INTERNAL::NarrowSignedIntegralToUnsignedIntegral<To, From>)
    {
        //Casting integral -> unsigned integral with decreased byte size is considered unsafe
        if(value < static_cast<From>(0) ||
            static_cast<To>(value) > static_cast<To>(std::numeric_limits<From>::max()))
        {
            TRAP_ASSERT(false, fmt::format("NumericCastError: Failed to cast {}() to type {}!", INTERNAL::TypeNameToString<From>(), value, INTERNAL::TypeNameToString<To>()));
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
#ifndef TRAP_NUMERICCASTS_H
#define TRAP_NUMERICCASTS_H

#include <concepts>
#include <exception>
#include <limits>
#include <string>

#include <fmt/format.h>

#include "Core/Backports.h"

template<typename T, typename U>
requires (std::is_arithmetic_v<T> && std::is_arithmetic_v<U>)
struct NarrowingError : public std::exception
{
    inline constexpr explicit NarrowingError(const U val)
        : Error(fmt::format("NarrowingError: Failed to cast {} to type {}, result got truncated!", val, TypeNameToString<T>()))
    {
    }

    [[nodiscard]] const char* what() const noexcept override
    {
        return Error.c_str();
    }

private:
    template<typename X>
    inline constexpr std::string TypeNameToString()
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

    std::string Error;
};

//-------------------------------------------------------------------------------------------------------------------//

/// <summary>
/// Narrow cast (demote) an arithmetic type to another smaller one without signedness promotions.
/// If the argument u cannot be represented in the target type T, then the function throws a NarrowingError.
/// </summary>
/// <returns>Narrowed value on success.</returns>
template<typename T, typename U>
requires ((sizeof(T) < sizeof(U) && //Destination type must be samller than the source type
          std::is_arithmetic_v<T> && std::is_arithmetic_v<U> && //Both types must be arithmetic
          std::is_signed_v<T> == std::is_signed_v<U>) ||  //Both types must have the same signedness
          (sizeof(T) <= sizeof(U) && std::floating_point<U> && std::signed_integral<T> && std::is_signed_v<U>))
[[nodiscard]] constexpr T NarrowCast(const U u)
{
    const T t = static_cast<T>(u);

    if(static_cast<U>(t) != u)
    {
        throw NarrowingError<T, U>(u);
    }

    return t;
}

//-------------------------------------------------------------------------------------------------------------------//

/// <summary>
/// Wide cast (promote) an arithmetic type to another bigger one without signedness promotions.
/// This function is guaranteed to never fail at runtime
/// </summary>
/// <returns>Widened value.</returns>
template<typename T, typename U>
requires ((sizeof(T) > sizeof(U) && //Destination type must be bigger than the source type
          std::is_arithmetic_v<T> && std::is_arithmetic_v<U> && //Both types must be arithmetic
          std::is_signed_v<T> == std::is_signed_v<U>) || //Both types must have the same signedness
          (sizeof(T) >= sizeof(U) && std::floating_point<T> && std::signed_integral<U> && std::is_signed_v<T>))
[[nodiscard]] constexpr T WideCast(const U u) noexcept
{
    return static_cast<T>(u);
}

//-------------------------------------------------------------------------------------------------------------------//

namespace INTERNAL
{
    template<typename T>
    using InvertedType = std::conditional_t<std::is_signed_v<T>, std::make_unsigned_t<T>, std::make_signed_t<T>>;
}

/// <summary>
/// Sign cast an integral type to/from unsigned to signed or vice versa.
/// If the argument u cannot be represented in the target type T, then the function throws a NarrowingError.
/// </summary>
/// <returns>Unsigned/Signed value on success.</returns>
template<typename U>
requires (std::integral<U>)
[[nodiscard]] constexpr INTERNAL::InvertedType<U> SignCast(const U u)
{
    const auto t = static_cast<INTERNAL::InvertedType<U>>(u);

    if(static_cast<U>(t) != u)
    {
        throw NarrowingError<INTERNAL::InvertedType<U>, U>(u);
    }

    if((t < INTERNAL::InvertedType<U>()) != (u < U()))
    {
        throw NarrowingError<INTERNAL::InvertedType<U>, U>(u);
    }

    return t;
}

#endif /*TRAP_NUMERICCASTS_H*/
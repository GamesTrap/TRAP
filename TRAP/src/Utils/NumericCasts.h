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

template<typename To, typename From>
requires std::is_arithmetic_v<To> && std::is_arithmetic_v<From>
[[nodiscard]] constexpr To NumericCast(const From value)
{
	static_assert(!std::is_enum_v<To> && !std::is_enum_v<From>, "CheckedCast(): Casting to/from enum is not allowed!");
	static_assert(!std::is_scoped_enum_v<To> && !std::is_scoped_enum_v<From>, "CheckedCast(): Casting to/from enum is not allowed!");
	static_assert(!std::same_as<To, From>, "CheckedCast(): Casting to the same type is not allowed!");

    //TODO Function local concepts to make the ifs simpler
    //     Only check problematic cases, safe cases should use the else branch!

    if constexpr(std::floating_point<To>)
    {
        if constexpr(std::floating_point<From>)
        {
            //Floating point -> floating point

            if constexpr(sizeof(To) > sizeof(From)) //Widening
            {
                //Casting floating point -> floating point with increased byte size is always considered safe
                return static_cast<To>(value);
            }
            else //Narrowing
            {
                //Casting floating point -> floating point with decreased byte size is considered unsafe
                //Note: Only bounds checks are done here, we ignore truncation caused by loss of precision
                if(value <= static_cast<From>(std::numeric_limits<To>::max()) &&
                   value >= static_cast<From>(std::numeric_limits<To>::lowest()))
                {
                    return static_cast<To>(value);
                }

                throw NarrowingError<To, From>(value);
            }
        }
        else
        {
             //Integral to floating point

             //Casting integral -> floating point is always considered safe
             //Note: We ignore truncation caused by loss of precision

             return static_cast<To>(value);
        }
    }
    else /*if constexpr(std::integral<To>)*/
    {
        if constexpr(std::floating_point<From>)
        {
            //Floating point to integral

            //Casting floating point -> integral is considered unsafe
            //Note: Only bounds checks are done here, we ignore truncation to integer values
            if(value <= static_cast<From>(std::numeric_limits<To>::max()) &&
               value >= static_cast<From>(std::numeric_limits<To>::lowest()))
            {
                return static_cast<To>(value);
            }

            throw NarrowingError<To, From>(value);
        }
        else
        {
            //Integral to integral

            if constexpr(sizeof(To) > sizeof(From)) //Widening
            {
                if constexpr(std::is_signed_v<To> == std::is_signed_v<From>) //Same signedness
                {
                    //Casting integral -> integral with increased byte size and same signedness is always considered safe
                    return static_cast<To>(value);
                }
                else //Different signedness
                {
                    if constexpr(std::is_signed_v<To> && std::is_unsigned_v<From>)
                    {
                        //Casting unsigned integral -> signed integral with increased byte size is always considered safe
                        return static_cast<To>(value);
                    }
                    else
                    {
                        //Casting integral -> unsigned integral with increased byte size is considered unsafe
                        if(value >= static_cast<From>(0))
                            return static_cast<To>(value);

                        throw NarrowingError<To, From>(value);
                    }
                }
            }
            else //Narrowing
            {
                if constexpr(std::is_signed_v<To> == std::is_signed_v<From>) //Same signedness
                {
                    //Casting integral -> integral with decreased byte size and same signedness is considered unsafe
                    if(value <= static_cast<From>(std::numeric_limits<To>::max()) &&
                       value >= static_cast<From>(std::numeric_limits<To>::lowest()))
                    {
                        return static_cast<To>(value);
                    }

                    throw NarrowingError<To, From>(value);
                }
                else //Different signedness
                {
                    if constexpr(std::is_signed_v<To> && std::is_unsigned_v<From>)
                    {
                        //Casting unsigned integral -> signed integral with decreased byte size is considered unsafe
                        if(value <= static_cast<From>(std::numeric_limits<To>::max()))
                        {
                            return static_cast<To>(value);
                        }

                        throw NarrowingError<To, From>(value);
                    }
                    else
                    {
                        //Casting integral -> unsigned integral with decreased byte size is considered unsafe
                        if(value >= static_cast<From>(0) &&
                           static_cast<To>(value) <= static_cast<To>(std::numeric_limits<From>::max()))
                        {
                            return static_cast<To>(value);
                        }

                        throw NarrowingError<To, From>(value);
                    }
                }
            }
        }
    }
}

#endif /*TRAP_NUMERICCASTS_H*/
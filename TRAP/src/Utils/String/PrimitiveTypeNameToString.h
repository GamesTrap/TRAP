#ifndef TRAP_PRIMITIVETYPENAMETOSTRING_H
#define TRAP_PRIMITIVETYPENAMETOSTRING_H

#include <string_view>
#include <type_traits>

#include "Core/Types.h"
#include "TRAP_Assert.h"

namespace TRAP::Utils::String
{
    template<typename X>
    requires std::is_fundamental_v<X>
    constexpr std::string_view PrimitiveTypeNameToString()
    {
        if constexpr(std::same_as<X, char>)
        {
            return "char";
        }
        else if constexpr(std::same_as<X, wchar_t>)
        {
            return "wchar_t";
        }
        else if constexpr(std::same_as<X, char8_t>)
        {
            return "char8_t";
        }
        else if constexpr(std::same_as<X, char16_t>)
        {
            return "char16_t";
        }
        else if constexpr(std::same_as<X, char32_t>)
        {
            return "char32_t";
        }
        else if constexpr(std::same_as<X, i8> || std::same_as<X, signed char>)
        {
            return "i8";
        }
        else if constexpr(std::same_as<X, u8> || std::same_as<X, unsigned char>)
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
        else if constexpr(std::same_as<X, i64> || std::same_as<X, long long>)
        {
            return "i64";
        }
        else if constexpr(std::same_as<X, u64> || std::same_as<X, unsigned long long>)
        {
            return "u64";
        }
#if __STDCPP_FLOAT16_T__ == 1
        else if constexpr(std::same_as<X, std::float16_t>)
        {
            return "f16";
        }
#endif /*__STDCPP_FLOAT16_T__ == 1*/
#if __STDCPP_FLOAT32_T__ == 1
        else if constexpr(std::same_as<X, std::float32_t>)
        {
            return "f32";
        }
#endif /*__STDCPP_FLOAT32_T__ == 1*/
        else if constexpr(std::same_as<X, f32>)
        {
            return "f32";
        }
#if __STDCPP_FLOAT64_T__ == 1
        else if constexpr(std::same_as<X, std::float64_t>)
        {
            return "f64";
        }
#endif /*__STDCPP_FLOAT64_T__ == 1*/
        else if constexpr(std::same_as<X, f64>)
        {
            return "f64";
        }
#if __STDCPP_FLOAT128_T__ == 1
        else if constexpr(std::same_as<X, std::float128_t>)
        {
            return "f128";
        }
#endif /*__STDCPP_FLOAT128_T__ == 1*/
#if __STDCPP_BFLOAT16_T__ == 1
        else if constexpr(std::same_as<X, std::bfloat16_t>)
        {
            return "bf16";
        }
#endif /*__STDCPP_BFLOAT16_T__ == 1*/
        else if constexpr(std::same_as<X, long double>)
        {
            if constexpr(sizeof(long double) == 8u)
            {
                return "f64";
            }
            else if constexpr(sizeof(long double) == 16u)
            {
                return "f128";
            }
            else
            {
                TRAP_ASSERT(false);
                return "Unknown Type";
            }
        }
        else if constexpr (std::same_as<X, unsigned long>)
        {
            if constexpr (sizeof(long) == 4u)
            {
                return "u32";
            }
            else if constexpr (sizeof(long) == 8u)
            {
                return "u64";
            }
            else
            {
                TRAP_ASSERT(false);
                return "Unknown Type";
            }
        }
        else if constexpr (std::same_as<X, long>)
        {
            if constexpr (sizeof(long) == 4u)
            {
                return "i32";
            }
            else if constexpr (sizeof(long) == 8u)
            {
                return "i64";
            }
            else
            {
                TRAP_ASSERT(false);
                return "Unknown Type";
            }
        }
        else if constexpr (std::same_as<X, bool>)
        {
            return "bool";
        }
        else if constexpr(std::same_as<X, void>)
        {
            return "void";
        }
        else if constexpr(std::same_as<X, std::nullptr_t>)
        {
            return "nullptr";
        }
        else
        {
            TRAP_ASSERT(false);
            return "Unknown Type";
        }
    }
}

#endif /*TRAP_PRIMITIVETYPENAMETOSTRING_H*/

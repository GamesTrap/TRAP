#ifndef TRAP_RANDOMINTERNAL_H
#define TRAP_RANDOMINTERNAL_H

#include <cstdint>
#include <iterator>

#include "Core/Types.h"

namespace TRAP::Utils::INTERNAL
{
    /// @brief True if type T is applicable by a std::uniform_int_distribution.
    /// @tparam T Type applicable by std::uniform_int_distribution.
    template<typename T>
    concept IsUniformInt = std::same_as<T, i16> ||
                           std::same_as<T, i32> ||
                           std::same_as<T, i64> ||
                           std::same_as<T, u16> ||
                           std::same_as<T, u32> ||
                           std::same_as<T, u64>;

    /// @brief True if type T is applicable by a std::uniform_real_distribution.
    /// @tparam T Type applicable by std::uniform_real_distribution.
    template<typename T>
    concept IsUniformReal = std::same_as<T,         f32> ||
                            std::same_as<T,         f64> ||
                            std::same_as<T, long double>;

    /// @brief True if type T is plain byte.
    /// @tparam T Byte type.
    template<typename T>
    concept IsByte = std::same_as<T, i8> ||
                     std::same_as<T, u8>;

    /// @brief True if type T is plain number type.
    /// @tparam T Number type.
    template<typename T>
    concept IsSupportedNumber = IsByte<T> || IsUniformReal<T> || IsUniformInt<T>;

    /// @brief True if type T is character type.
    /// @tparam T Character type.
    template<typename T>
    concept IsSupportedCharacter = std::same_as<T,     char> ||
                                   std::same_as<T,  wchar_t> ||
                                   std::same_as<T, char16_t> ||
                                   std::same_as<T, char32_t>;

    /// @brief True if type T is iterator
    /// @tparam T Iterator type.
    template<typename T>
    concept IsIterator = requires
    {
        typename std::iterator_traits<T>::difference_type;
        typename std::iterator_traits<T>::pointer;
        typename std::iterator_traits<T>::reference;
        typename std::iterator_traits<T>::value_type;
        typename std::iterator_traits<T>::iterator_category;
    };
}

#endif /*TRAP_RANDOMINTERNAL_H*/

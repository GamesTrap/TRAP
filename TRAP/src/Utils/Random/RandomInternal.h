#ifndef TRAP_RANDOMINTERNAL_H
#define TRAP_RANDOMINTERNAL_H

#include <cstdint>
#include <iterator>

#include "Core/Types.h"

namespace TRAP::Utils::INTERNAL
{
    /// @brief Key type for getting common type numbers or objects.
    struct Common { };

    /// @brief True if type T is applicable by a std::uniform_int_distribution.
    /// @tparam T Type applicable by std::uniform_int_distribution.
    template<typename T>
    concept IsUniformInt = std::same_as<T,  i16> ||
                           std::same_as<T,  i32> ||
                           std::same_as<T,  i64> ||
                           std::same_as<T, u16> ||
                           std::same_as<T, u32> ||
                           std::same_as<T, u64>;

    /// @brief True if type T is applicable by a std::uniform_real_distribution.
    /// @tparam T Type applicable by std::uniform_real_distribution.
    template<typename T>
    concept IsUniformReal = std::same_as<T,       f32> ||
                            std::same_as<T,      f64> ||
                            std::same_as<T, long double>;

    /// @brief True if type T is plain byte.
    /// @tparam T Byte type.
    template<typename T>
    concept IsByte = std::same_as<T,  i8> ||
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
    struct IsIterator
    {
    private:
        static char Test(...) noexcept { return 0; }

        template <typename U, typename = typename std::iterator_traits<U>::difference_type,
                              typename = typename std::iterator_traits<U>::pointer,
                              typename = typename std::iterator_traits<U>::reference,
                              typename = typename std::iterator_traits<U>::value_type,
                              typename = typename std::iterator_traits<U>::iterator_category>
        static i64 Test([[maybe_unused]] U&& other) noexcept { return 0; }
    public:
        static constexpr bool value = std::same_as<decltype(Test(std::declval<T>())), i64>;
    };
}

#endif /*TRAP_RANDOMINTERNAL_H*/

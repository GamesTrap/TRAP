#ifndef TRAP_RANDOMINTERNAL_H
#define TRAP_RANDOMINTERNAL_H

#include <cstdint>
#include <iterator>

namespace TRAP::Utils::INTERNAL
{
    /// <summary>
    /// Key type for getting common type numbers or objects.
    /// </summary>
    struct Common { };

    /// <summary>
    /// True if type T is applicable by a std::uniform_int_distribution.
    /// </summary>
    /// <typeparam name="T">Type applicable by std::uniform_int_distribution.</typeparam>
    template<typename T>
    concept IsUniformInt = std::same_as<T,  int16_t> ||
                           std::same_as<T,  int32_t> ||
                           std::same_as<T,  int64_t> ||
                           std::same_as<T, uint16_t> ||
                           std::same_as<T, uint32_t> ||
                           std::same_as<T, uint64_t>;

    /// <summary>
    /// True if type T is applicable by a std::uniform_real_distribution.
    /// </summary>
    /// <typeparam name="T">Type applicable by std::uniform_real_distribution.</typeparam>
    template<typename T>
    concept IsUniformReal = std::same_as<T,       float> ||
                            std::same_as<T,      double> ||
                            std::same_as<T, long double>;

    /// <summary>
    /// True if type T is plain byte.
    /// </summary>
    /// <typeparam name="T">Byte type.</typeparam>
    template<typename T>
    concept IsByte = std::same_as<T,  int8_t> ||
                     std::same_as<T, uint8_t>;

    /// <summary>
    /// True if type T is plain number type.
    /// </summary>
    /// <typeparam name="T">Number type.</typeparam>
    template<typename T>
    concept IsSupportedNumber = IsByte<T> || IsUniformReal<T> || IsUniformInt<T>;

    /// <summary>
    /// True if type T is character type.
    /// </summary>
    /// <typeparam name="T">Character type.</typeparam>
    template<typename T>
    concept IsSupportedCharacter = std::same_as<T,     char> ||
                                   std::same_as<T,  wchar_t> ||
                                   std::same_as<T, char16_t> ||
                                   std::same_as<T, char32_t>;

    /// <summary>
    /// True if type T is iterator
    /// </summary>
    /// <typeparam name="T">Iterator type.</typeparam>
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
        static long Test([[maybe_unused]] U&& other) noexcept { return 0; }
    public:
        static constexpr bool value = std::same_as<decltype(Test(std::declval<T>())), long>;
    };
}

#endif /*TRAP_RANDOMINTERNAL_H*/
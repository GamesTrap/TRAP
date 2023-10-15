#ifndef TRAP_BACKPORTS_H
#define TRAP_BACKPORTS_H

#include <utility>
#include <version>
#include <bit>
#include <array>
#include <ranges>
#include <algorithm>

#ifndef __cpp_lib_is_scoped_enum

namespace std
{
    /// <summary>
    /// Checks whether E is a scoped enumeration type.
    /// Provides the member constant value which is equal to true, if E is a scoped enumeration type.
    /// Otherwise, value is equal to false.
    /// </summary>
    /// <typeparam name="E">Type to check.</typeparam>
    template<typename E>
    struct is_scoped_enum : std::bool_constant<requires
    {
        requires std::is_enum_v<E>;
        requires !std::is_convertible_v<E, std::underlying_type_t<E>>;
    }>
    {};

    /// <summary>
    /// Check whether E is a scoped enumeration type.
    /// True if E is a scoped enumeration type, false otherwise.
    /// </summary>
    /// <typeparam name="T">Type to check.</typeparam>
    template<class T>
    inline constexpr bool is_scoped_enum_v = is_scoped_enum<T>::value;
}

#endif

#ifndef __cpp_lib_to_underlying

namespace std
{
    /// <summary>
    /// Retrieve a value of Enum using its underlying data type.
    /// </summary>
    /// <typeparam name="Enum">Enum to get value from.</typeparam>
    /// <param name="e">Enum value to retrieve.</param>
    /// <returns>Enum value represented with its underlying data type.</returns>
    template<class Enum>
    requires std::is_enum_v<Enum> || std::is_scoped_enum_v<Enum>
    constexpr std::underlying_type_t<Enum> to_underlying(Enum e) noexcept
    {
        return static_cast<std::underlying_type_t<Enum>>(e);
    }
}

#endif /*__cpp_lib_to_underlying*/

#ifndef __cpp_lib_byteswap

namespace std
{
    /// <summary>
    /// Reverses the bytes in the given integer value n.
    /// </summary>
    /// <param name="n">Integer value.</param>
    /// <returns>
    /// An integer value of type T whose object representation comprises
    /// the bytes of that of n in reversed order.
    /// </returns>
    constexpr std::integral auto byteswap(std::integral auto n) noexcept
    {
        static_assert(std::has_unique_object_representations_v<decltype(n)>, "T may not have padding bits!");
        auto valueRepresentation = std::bit_cast<std::array<std::byte, sizeof(decltype(n))>>(n);
        std::ranges::reverse(valueRepresentation);
        return std::bit_cast<decltype(n)>(valueRepresentation);
    }
}

#endif /*__cpp_lib_byteswap*/

#ifndef __cpp_lib_ranges_contains

namespace std::ranges
{
    struct __contains_fn
    {
        template<std::input_iterator I, std::sentinel_for<I> S, typename T, typename Proj = std::identity>
        requires std::indirect_binary_predicate<ranges::equal_to, std::projected<I, Proj>, const T*>
        constexpr bool operator()(I first, S last, const T& value, Proj proj = {}) const
        {
            return std::ranges::find(std::move(first), last, value, std::move(proj)) != last;
        }

        template<ranges::input_range R, typename T, typename Proj = std::identity>
        requires std::indirect_binary_predicate<ranges::equal_to, std::projected<ranges::iterator_t<R>, Proj>, const T*>
        constexpr bool operator()(R&& r, const T& value, Proj proj = {}) const
        {
            return (*this)(std::ranges::begin(r), std::ranges::end(r), value, std::move(proj));
        }
    };

    inline constexpr __contains_fn contains{};
}

#endif /*__cpp_lib_ranges_contains*/

#ifndef __cpp_lib_unreachable

namespace std
{
    [[noreturn]] inline void unreachable()
    {
    #ifdef __GNUC__
        __builtin_unreachable();
    #elif defined(_MSC_VER)
        __assume(false);
    #endif
    }
}

#endif /*__cpp_lib_unreachable*/

#endif /*TRAP_BACKPORTS_H*/
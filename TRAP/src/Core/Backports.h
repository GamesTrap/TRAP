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
    /// @brief Checks whether E is a scoped enumeration type.
    ///        Provides the member constant value which is equal to true, if E is a scoped enumeration type.
    ///        Otherwise, value is equal to false.
    /// @tparam E Type to check.
    template<typename E>
    struct is_scoped_enum : std::bool_constant<requires
    {
        requires std::is_enum_v<E>;
        requires !std::is_convertible_v<E, std::underlying_type_t<E>>;
    }>
    {};

    /// @brief Check whether E is a scoped enumeration type. True if E is a scoped enumeration type, false otherwise.
    /// @tparam T Type to check.
    template<class T>
    inline constexpr bool is_scoped_enum_v = is_scoped_enum<T>::value;
}

#else

#if __cplusplus >= 202302L
#warning "Backported std::is_scoped_enum and std::is_scoped_enum_v should be removed!"
#endif /*__cplusplus >= 202302L*/

#endif /*__cpp_lib_is_scoped_enum*/

#ifndef __cpp_lib_to_underlying

namespace std
{
    /// @brief Retrieve a value of Enum using its underlying data type.
    /// @tparam Enum Enum to get value from.
    /// @param e Enum value to retrieve.
    /// @return Enum value represented with its underlying data type.
    template<class Enum>
    requires std::is_enum_v<Enum> || std::is_scoped_enum_v<Enum>
    constexpr std::underlying_type_t<Enum> to_underlying(Enum e) noexcept
    {
        return static_cast<std::underlying_type_t<Enum>>(e);
    }
}

#else

#if __cplusplus >= 202302L
#warning "Backported std::to_underlying() should be removed!"
#endif /*__cplusplus >= 202302L*/

#endif /*__cpp_lib_to_underlying*/

#ifndef __cpp_lib_byteswap

namespace std
{
    /// @brief Reverses the bytes in the given integer value n.
    /// @param n Integer value.
    /// @return An integer value of type T whose object representation comprises the bytes of that of n in reversed order.
    constexpr std::integral auto byteswap(std::integral auto n) noexcept
    {
        static_assert(std::has_unique_object_representations_v<decltype(n)>, "T may not have padding bits!");
        auto valueRepresentation = std::bit_cast<std::array<std::byte, sizeof(decltype(n))>>(n);
        std::ranges::reverse(valueRepresentation);
        return std::bit_cast<decltype(n)>(valueRepresentation);
    }
}

#else

#if __cplusplus >= 202302L
#warning "Backported std::byteswap() should be removed!"
#endif /*__cplusplus >= 202302L*/

#endif /*__cpp_lib_byteswap*/

#ifndef __cpp_lib_ranges_contains

namespace std::ranges
{
    struct __contains_fn
    {
        /// @brief Search-based algorithm that checks whether or not a given range contains a value with
        ///        iterator-sentinel pairs.
        /// @param first The range of elements to examine.
        /// @param last The range of elements to examine.
        /// @param value Value to compare the elements to.
        /// @param proj Projection to apply to the elements.
        /// @return std::ranges::find(std::move(first), last, value, proj) != last.
        template<std::input_iterator I, std::sentinel_for<I> S, typename T, typename Proj = std::identity>
        requires std::indirect_binary_predicate<ranges::equal_to, std::projected<I, Proj>, const T*>
        constexpr bool operator()(I first, S last, const T& value, Proj proj = {}) const
        {
            return std::ranges::find(std::move(first), last, value, std::move(proj)) != last;
        }

        /// @brief Search-based algorithm that checks whether or not a given range contains a value with
        ///        iterator-sentinel pairs.
        /// @param r The range of the elements to examine.
        /// @param value Value to compare the elements to.
        /// @param proj Projection to apply to the elements.
        /// @return std::ranges::find(std::move(first), last, value, proj) != last.
        template<ranges::input_range R, typename T, typename Proj = std::identity>
        requires std::indirect_binary_predicate<ranges::equal_to, std::projected<ranges::iterator_t<R>, Proj>, const T*>
        constexpr bool operator()(R&& r, const T& value, Proj proj = {}) const
        {
            return (*this)(std::ranges::begin(r), std::ranges::end(r), value, std::move(proj));
        }
    };

    inline constexpr __contains_fn contains{};
}

#else

#if __cplusplus >= 202302L
#warning "Backported std::ranges::contains() should be removed!"
#endif /*__cplusplus >= 202302L*/

#endif /*__cpp_lib_ranges_contains*/

#ifndef __cpp_lib_unreachable

namespace std
{
    /// @brief Invokes undefined behaviour. An implementation may use this to optimize impossible code branches away
    ///        (typically, in optimized builds) or to trap them to prevent further execution (typically, in debug builds).
    [[noreturn]] inline void unreachable()
    {
    #ifdef __GNUC__
        __builtin_unreachable();
    #elif defined(_MSC_VER)
        __assume(false);
    #endif
    }
}

#else

#if __cplusplus >= 202302L
#warning "Backported std::unreachable() should be removed!"
#endif /*__cplusplus >= 202302L*/

#endif /*__cpp_lib_unreachable*/

#endif /*TRAP_BACKPORTS_H*/
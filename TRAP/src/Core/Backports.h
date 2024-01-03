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

#ifndef __cpp_lib_debugging

namespace std
{
    /// @brief Attempts to determine if the program is being executed with the debugger present.
    ///        Formally, the behaviour of this function is completely implementation-defined.
    /// @return True if, to the best knowledge, the program is executed under a debugger.
    /// @note The intent of this function is allowing printing out extra output to help diagnose
    ///       problems, executing extra test code, displaying an extra user interface to help in
    ///       debugging, etc.
    [[nodiscard]] bool is_debugger_present() noexcept;

    /// @brief Unconditional breakpoint: attempts to temporarily halt the execution
    ///        of the program and transfer control to the debugger whether or not it's
    ///        not possible to determine if the debugger is present. Formally, the behaviour
    ///        of this function is completely implementation-defined.
    /// @note The intent of this function is allowing for runtime control of breakpoints
    ///       beyond what might be available from a debugger while not causing the program
    ///       to exit. For example, breaking when an infrequent non-critical condition is
    ///       detected, allowing programmatic control with complex runtime sensitive conditions,
    ///       breaking on user input to inspect context in interactive programs without needing
    ///       to switch to the debugger application, etc.
    void breakpoint() noexcept;

    /// @brief Conditional breakpoint: attempts to temporarily halt
    ///        the execution of the program and transfer control to the
    ///        debugger if it were able to determine that the debugger is present.
    ///        Acts as a no-op otherwise. Formally, the behaviour of this function is
    ///        completely implementation-defined.
    void breakpoint_if_debugging() noexcept;
}

#else

#if __cplusplus >= 202311L
#warning "Backported std::breakpoint() should be removed!"
#warning "Backported std::breakpoint_if_debugging() should be removed!"
#warning "Backported std::is_debugger_present() should be removed!"
#endif /*__cplusplus >= 202311L*/

#endif /*__cpp_lib_debugging*/

#if __cpp_lib_containers_ranges < 202202L

template<typename Range, typename T>
concept ContainerCompatibleRange = std::ranges::input_range<Range> && std::convertible_to<std::ranges::range_reference_t<Range>, T>;

namespace INTERNAL
{
    template<typename Container>
    concept ContainerHasInsertMemberFunction = requires(Container c, typename Container::iterator it)
    {
        {
            c.insert(it, c.begin(), c.end())
        } -> std::same_as<typename Container::iterator>;
    };

    template<typename Container>
    concept ContainerHasEndIterator = requires(Container c) {c.end();};

    template<typename Container>
    concept ContainerHasAssignMemberFunction = requires(Container c)
    {
        {
            c.assign(c.begin(), c.end())
        } -> std::same_as<void>;
    };
}

template<typename Container, ContainerCompatibleRange<typename Container::value_type> Range>
requires INTERNAL::ContainerHasInsertMemberFunction<Container>
constexpr typename Container::iterator ContainerInsertRange(Container& container, typename Container::const_iterator position, Range&& range)
{
    return container.insert(position, std::ranges::begin(range), std::ranges::end(range));
}

template<typename Container, ContainerCompatibleRange<typename Container::value_type> Range>
requires INTERNAL::ContainerHasEndIterator<Container>
constexpr void ContainerAppendRange(Container& container, Range&& range)
{
    ContainerInsertRange(container, container.end(), std::forward<Range>(range));
}

template<typename Container, ContainerCompatibleRange<typename Container::value_type> Range>
requires INTERNAL::ContainerHasAssignMemberFunction<Container>
constexpr void ContainerAssignRange(Container& container, Range&& range)
{
    container.assign(std::ranges::begin(range), std::ranges::end(range));
}

#else

#warning "Backported std::<Container>::assign_range() should be removed!"
#warning "Backported std::<Container>::insert_range() should be removed!"
#warning "Backported std::<Container>::append_range() should be removed!"

#endif

#endif /*TRAP_BACKPORTS_H*/
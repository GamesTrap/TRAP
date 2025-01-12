#ifndef TRAP_BACKPORTS_RANGESCONTAINS_H
#define TRAP_BACKPORTS_RANGESCONTAINS_H

#ifndef __cpp_lib_ranges_contains

#include <algorithm>
#include <functional>
#include <iterator>
#include <ranges>
#include <utility>

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

        //-------------------------------------------------------------------------------------------------------------------//

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

    constexpr __contains_fn contains{};
}

#else

#if __cplusplus >= 202302L
#warning "Backported std::ranges::contains() should be removed!"
#endif /*__cplusplus >= 202302L*/

#endif /*__cpp_lib_ranges_contains*/

#endif /*TRAP_BACKPORTS_RANGESCONTAINS_H*/

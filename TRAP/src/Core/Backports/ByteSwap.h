#ifndef TRAP_BACKPORTS_BYTESWAP_H
#define TRAP_BACKPORTS_BYTESWAP_H

#ifndef __cpp_lib_byteswap

#include <algorithm>
#include <bit>
#include <concepts>
#include <type_traits>

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

#endif /*TRAP_BACKPORTS_BYTESWAP_H*/

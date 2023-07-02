#ifndef TRAP_BACKPORTS_H
#define TRAP_BACKPORTS_H

#include <utility>
#include <version>
#include <bit>
#include <array>
#include <ranges>
#include <algorithm>

#ifndef __cpp_lib_to_underlying

namespace std
{
    /// <summary>
    /// Retrieve a value of Enum using its underlying data type.
    /// </summary>
    /// <typeparam name="Enum">Enum to get value from</typeparam>
    /// <param name="e">Enum value to retrieve.</param>
    /// <returns>Enum value represented with its underlying data type.</returns>
    template<class Enum>
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
    template<std::integral T>
    constexpr T byteswap(T n) noexcept
    {
        static_assert(std::has_unique_object_representations_v<T>, "T may not have padding bits!");
        auto valueRepresentation = std::bit_cast<std::array<std::byte, sizeof(T)>>(n);
        std::ranges::reverse(valueRepresentation);
        return std::bit_cast<T>(valueRepresentation);
    }
}

#endif /*__cpp_lib_byteswap*/

#endif /*TRAP_BACKPORTS_H*/
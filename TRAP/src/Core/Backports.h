#ifndef TRAP_BACKPORTS_H
#define TRAP_BACKPORTS_H

#include <utility>
#include <version>

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

#endif /*TRAP_BACKPORTS_H*/
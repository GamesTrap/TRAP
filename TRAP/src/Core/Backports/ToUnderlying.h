#ifndef TRAP_BACKPORTS_TOUNDERLYING_H
#define TRAP_BACKPORTS_TOUNDERLYING_H

#ifndef __cpp_lib_to_underlying

#include "IsScopedEnum.h"

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

#endif /*TRAP_BACKPORTS_TOUNDERLYING_H*/

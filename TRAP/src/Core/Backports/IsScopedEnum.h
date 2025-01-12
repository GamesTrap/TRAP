#ifndef TRAP_BACKPORTS_ISSCOPEDENUM_H
#define TRAP_BACKPORTS_ISSCOPEDENUM_H

#ifndef __cpp_lib_is_scoped_enum

#include <type_traits>

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
    constexpr bool is_scoped_enum_v = is_scoped_enum<T>::value;
}

#else

#if __cplusplus >= 202302L
#warning "Backported std::is_scoped_enum and std::is_scoped_enum_v should be removed!"
#endif /*__cplusplus >= 202302L*/

#endif /*__cpp_lib_is_scoped_enum*/

#endif /*TRAP_BACKPORTS_ISSCOPEDENUM_H*/

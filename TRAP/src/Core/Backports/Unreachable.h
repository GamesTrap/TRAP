#ifndef TRAP_BACKPORTS_UNREACHABLE_H
#define TRAP_BACKPORTS_UNREACHABLE_H

#ifndef __cpp_lib_unreachable

namespace std
{
    /// @brief Invokes undefined behaviour. An implementation may use this to optimize impossible code branches away
    ///        (typically, in optimized builds) or to trap them to prevent further execution (typically, in debug builds).
    [[noreturn]] void unreachable();
}

#else

#if __cplusplus >= 202302L
#warning "Backported std::unreachable() should be removed!"
#endif /*__cplusplus >= 202302L*/

#endif /*__cpp_lib_unreachable*/

#endif /*TRAP_BACKPORTS_UNREACHABLE_H*/

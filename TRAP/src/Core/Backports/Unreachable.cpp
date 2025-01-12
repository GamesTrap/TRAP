#include "TRAPPCH.h"
#include "Unreachable.h"

#ifndef __cpp_lib_unreachable

namespace std
{
    [[noreturn]] void unreachable()
    {
#ifdef __GNUC__
        __builtin_unreachable();
#elif defined(_MSC_VER)
        __assume(false);
#endif
    }
}

#endif /*__cpp_lib_unreachable*/

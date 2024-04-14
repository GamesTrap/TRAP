#ifndef TRAP_FUTURES_ISFUTURETYPE_H
#define TRAP_FUTURES_ISFUTURETYPE_H

#include <future>

namespace TRAP::Utils
{
    template<typename T>
    struct IsFutureType
    {
        static constexpr bool value = false;
    };

    template<typename T>
    struct IsFutureType<std::future<T>>
    {
        static constexpr bool value = true;
    };

    template<typename T>
    struct IsFutureType<std::shared_future<T>>
    {
        static constexpr bool value = true;
    };
}

#endif /*TRAP_FUTURES_ISFUTURETYPE_H*/

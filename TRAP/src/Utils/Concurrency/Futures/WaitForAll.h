#ifndef TRAP_FUTURES_WAITFORALL_H
#define TRAP_FUTURES_WAITFORALL_H

//Tracy - Profiler
#include <tracy/Tracy.hpp>

#include "IsFutureType.h"

namespace TRAP::Utils
{
    /// @brief Wait until all std::future or std::shared_futures are ready.
    /// @note This call blocks until all futures are ready.
    template<typename F1, typename... Fs>
    requires (IsFutureType<F1>::value && IsFutureType<Fs...>::value )
    void WaitForAll(F1& f1, Fs&... fs)
    {
        ZoneScoped;

        f1.wait();
        (fs.wait(), ...);
    }

    /// @brief Wait until all std::future or std::shared_futures are ready.
    /// @note This call blocks until all futures are ready.
    template<typename Iterator>
    requires (IsFutureType<typename Iterator::value_type>::value)
    void WaitForAll(Iterator begin, Iterator end)
    {
        ZoneScoped;

        for(Iterator current = begin; current != end; ++current)
            current->wait();
    }

    /// @brief Wait until all std::future or std::shared_futures are ready.
    /// @note This call blocks until all futures are ready.
    template<std::ranges::input_range R>
    void WaitForAll(R&& r)
    {
        WaitForAll(std::ranges::begin(r), std::ranges::end(r));
    }
}

#endif /*TRAP_FUTURES_WAITFORALL_H*/

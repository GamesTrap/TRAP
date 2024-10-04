#ifndef TRAP_FUTURES_ISFUTURETYPE_H
#define TRAP_FUTURES_ISFUTURETYPE_H

#include <future>

namespace TRAP::Utils
{
    namespace INTERNAL
    {
        template <typename T>
        struct is_future_type_impl : std::false_type {};

        template <typename T>
        struct is_future_type_impl<std::future<T>> : std::true_type {};

        template <typename T>
        struct is_future_type_impl<std::shared_future<T>> : std::true_type {};
    }

    template<typename T>
    concept IsFutureType = INTERNAL::is_future_type_impl<T>::value;
}

#endif /*TRAP_FUTURES_ISFUTURETYPE_H*/

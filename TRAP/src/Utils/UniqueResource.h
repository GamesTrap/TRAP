#ifndef TRAP_UNIQUE_RESOURCE_H
#define TRAP_UNIQUE_RESOURCE_H

//This implementation is based on C++ Library fundamentals TS v3

#include <exception>
#include <limits>
#include <type_traits>
#include <utility>

#include "Core/Types.h"

namespace TRAP
{
    namespace INTERNAL
    {
        template<typename T, typename U>
        concept NotSameAs = !std::same_as<T, U>;

        template<typename T>
        concept NotLValueReference = !std::is_lvalue_reference_v<T>;

#ifdef _MSC_VER
        struct MSVCFriendNoexceptWorkaroundUniqueResource;
#endif /*_MSC_VER*/
    }

    template<typename EF>
    class ScopeFail
    {
    public:
        template<typename EFPtr>
        requires INTERNAL::NotSameAs<std::remove_cvref_t<EFPtr>, ScopeFail> && std::constructible_from<EF, EFPtr>
        constexpr explicit ScopeFail(EFPtr&& f) noexcept(std::is_nothrow_constructible_v<EF, EFPtr&>)
#ifdef __cpp_exceptions
        try
#endif /*__cpp_exceptions*/
            : m_exitFunction(f)
        {
        }
#ifdef __cpp_exceptions
        catch(...)
        {
            f();
        }
#endif /*__cpp_exceptions*/

        template<typename EFPtr>
        requires INTERNAL::NotSameAs<std::remove_cvref_t<EFPtr>, ScopeFail> && std::constructible_from<EF, EFPtr> &&
                 INTERNAL::NotLValueReference<EFPtr> && std::is_nothrow_constructible_v<EF, EFPtr>
        constexpr explicit ScopeFail(EFPtr&& f) noexcept
            : m_exitFunction(std::forward<EFPtr>(f))
        {
        }

        constexpr ScopeFail(ScopeFail&& rhs) noexcept
        requires std::is_nothrow_move_constructible_v<EF>
            : m_exitFunction(std::forward<EF>(rhs.m_exitFunction))
        {
            rhs.Release();
        }

        constexpr ScopeFail(ScopeFail&& rhs) noexcept(std::is_nothrow_copy_constructible_v<EF>)
        requires (!std::is_nothrow_move_constructible_v<EF>) && std::is_copy_constructible_v<EF>
            : m_exitFunction(rhs.m_exitFunction)
        {
            rhs.Release();
        }

        consteval ScopeFail(const ScopeFail&) noexcept = delete;
        consteval ScopeFail& operator=(const ScopeFail&) noexcept = delete;
        consteval ScopeFail& operator=(ScopeFail&&) noexcept = delete;

        ~ScopeFail() noexcept(noexcept(this->m_exitFunction))
        {
            if(std::uncaught_exceptions() > m_uncaughtInit)
                m_exitFunction();
        }

        constexpr void Release() noexcept
        {
            m_uncaughtInit = std::numeric_limits<i32>::max();
        }

    private:
        [[no_unique_address]] EF m_exitFunction;
        i32 m_uncaughtInit = std::uncaught_exceptions();
    };

    template<typename EF>
    ScopeFail(EF) -> ScopeFail<EF>;

    template<typename Resource, typename Deleter>
    class UniqueResource
    {
        static_assert(!std::is_rvalue_reference_v<Resource>);
        static_assert(!std::is_reference_v<Deleter>);

        struct Dummy
        {
            constexpr void Release()
            {}
        };

        template<typename T>
        struct Wrap
        {
            template<typename U>
            requires std::is_constructible_v<T, U>
            constexpr Wrap(U&&) noexcept(std::is_nothrow_constructible_v<T, U>);

            template<typename U, typename Deleter2>
            requires std::is_constructible_v<T, U>
            constexpr Wrap(U&& r, Deleter2&& d) noexcept(std::is_nothrow_constructible_v<T, U>)
                : m_resource(std::forward<U>(r))
            {
                d.Release();
            }

            constexpr Wrap() = default;

            constexpr ~Wrap() = default;

            constexpr Wrap(Wrap&&) = default;

            constexpr Wrap(Wrap&& rhs) noexcept(std::is_nothrow_constructible_v<T, T&>)
            requires (!std::is_nothrow_move_constructible_v<T>)
                : m_resource(rhs.m_resource)
            {
            }

            consteval Wrap(const Wrap&) noexcept = delete;

            constexpr Wrap& operator=(const Wrap&) = default;
            constexpr Wrap& operator=(Wrap&&) = default;

            [[nodiscard]] constexpr T& Get() noexcept
            {
                return m_resource;
            }
            [[nodiscard]] constexpr const T& Get() const noexcept
            {
                return m_resource;
            }

            [[no_unique_address]] T m_resource{};
        };

        template<typename T>
        struct Wrap<T&>
        {
            template<typename U>
            requires std::is_constructible_v<std::reference_wrapper<T>, U>
            constexpr Wrap(U&&) noexcept(std::is_nothrow_constructible_v<std::reference_wrapper<T>, U>);

            template<typename U, typename Deleter2>
            constexpr Wrap(U&& r, Deleter2&& d) noexcept(std::is_nothrow_constructible_v<std::reference_wrapper<T>, U>)
                : m_resourcePtr(std::addressof(static_cast<T&>(r)))
            {
                d.Release();
            }

            // consteval Wrap() noexcept = delete;
            //constexpr Wrap() = default;

            constexpr ~Wrap() = default;

            consteval Wrap(Wrap&&) noexcept = delete;
            constexpr Wrap(const Wrap&) = default;

            consteval Wrap& operator=(Wrap&&) noexcept = delete;
            constexpr Wrap& operator=(const Wrap&) = default;

            [[nodiscard]] constexpr T& Get() noexcept
            {
                return *m_resourcePtr;
            }

            [[nodiscard]] constexpr const T& Get() const noexcept
            {
                return *m_resourcePtr;
            }

            T* m_resourcePtr = nullptr;
        };

        using Resource1 = Wrap<Resource>;

        template<typename T, typename U>
        requires std::is_constructible_v<T, U> && (std::is_nothrow_constructible_v<T, U> || std::is_constructible_v<T, U&>)
        using ForwardT = std::conditional_t<std::is_nothrow_constructible_v<T, U>, U, U&>;

        template<typename T, typename U>
        [[nodiscard]] static constexpr ForwardT<T, U> Forward(U& u)
        {
            return static_cast<ForwardT<T, U>&&>(u);
        }

        template<typename T, typename U, typename Deleter2, typename Resource2>
        [[nodiscard]] static constexpr auto Guard(Deleter2& d, Resource2& r)
        {
            if constexpr(std::is_nothrow_constructible_v<T, U>)
                return Dummy{};
            else
                return ScopeFail{[&]{d(r);}};
        }

    public:
        constexpr UniqueResource() = default;

        template<typename Resource2, typename Deleter2>
        requires requires
        {
            typename ForwardT<Resource1, Resource2>;
            typename ForwardT<Deleter, Deleter2>;
        }
        constexpr UniqueResource(Resource2&& r, Deleter2&& d) noexcept((std::is_nothrow_constructible_v<Resource1, Resource2> || std::is_nothrow_constructible_v<Resource1, Resource2&>) &&
                                                                       (std::is_nothrow_constructible_v<Deleter, Deleter2> || std::is_nothrow_constructible_v<Deleter, Deleter2&>))
            : m_resource(Forward<Resource1, Resource2>(r), Guard<Resource1, Resource2>(d, r)),
              m_deleter(Forward<Deleter, Deleter2>(d), Guard<Deleter, Deleter2>(d, m_resource.Get())),
              m_execOnReset(true)
        {
        }

        constexpr UniqueResource(UniqueResource&& rhs) noexcept
        requires std::is_nothrow_move_constructible_v<Resource1> && std::is_nothrow_move_constructible_v<Deleter>
            : m_resource(std::move(rhs.m_resource)),
              m_deleter(std::move(rhs.m_deleter)),
              m_execOnReset(std::exchange(rhs.m_execOnReset, false))
        {
        }

        constexpr UniqueResource(UniqueResource&& rhs)
        requires std::is_nothrow_move_constructible_v<Resource1> && (!std::is_nothrow_move_constructible_v<Deleter>)
            : m_resource(std::move(rhs.m_resource)),
              m_deleter(Forward<Deleter, Deleter>(rhs.m_deleter.Get()), ScopeFail([&]
              {
                  if(rhs.m_execOnReset)
                  {
                      rhs.m_deleter.Get()(m_resource.Get());
                      rhs.Release();
                  }
              })),
              m_execOnReset(std::exchange(rhs.m_execOnReset, false))
        {
        }

        constexpr UniqueResource(UniqueResource&& rhs)
        requires (!std::is_nothrow_move_constructible_v<Resource1>)
            : UniqueResource(rhs.m_resource.Get(), rhs.m_deleter.Get(), Dummy{})
        {
            if(rhs.m_execOnReset)
            {
                m_execOnReset = true;
                rhs.m_execOnReset = false;
            }
        }

        constexpr ~UniqueResource()
        {
            Reset();
        }

        constexpr UniqueResource& operator=(UniqueResource&& rhs) noexcept(std::is_nothrow_move_assignable_v<Resource1> && std::is_nothrow_move_assignable_v<Deleter>)
        {
            Reset();
            if constexpr(std::is_nothrow_move_assignable_v<Resource1>)
            {
                if constexpr(std::is_nothrow_move_assignable_v<Deleter>)
                {
                    m_resource = std::move(rhs.m_resource);
                    m_deleter = std::move(rhs.m_deleter);
                }
                else
                {
                    m_deleter = rhs.m_deleter;
                    m_resource = std::move(rhs.m_resource);
                }
            }
            else
            {
                if constexpr(std::is_nothrow_move_assignable_v<Deleter>)
                {
                    m_resource = rhs.m_resource;
                    m_deleter = std::move(rhs.m_deleter);
                }
                else
                {
                    m_resource = rhs.m_resource;
                    m_deleter = rhs.m_deleter;
                }
            }
            m_execOnReset = std::exchange(rhs.m_execOnReset, false);
            return *this;
        }

        constexpr void Reset() noexcept
        {
            if(m_execOnReset)
            {
                m_execOnReset = false;
                m_deleter.Get()(m_resource.Get());
            }
        }

        template<typename Resource2>
        constexpr void Reset(Resource2&& r)
        {
            Reset();
            if constexpr(std::is_nothrow_assignable_v<Resource1&, Resource2>)
                m_resource.Get() = std::forward<Resource2>(r);
            else
                m_resource.Get() = const_cast<const std::remove_reference_t<Resource2>&>(r);
            m_execOnReset = true;
        }

        constexpr void Release() noexcept
        {
            m_execOnReset = false;
        }

        [[nodiscard]] constexpr const Resource& Get() const noexcept
        {
            return m_resource.Get();
        }

        [[nodiscard]] constexpr std::add_lvalue_reference_t<std::remove_pointer_t<Resource>> operator*() const noexcept
        requires std::is_pointer_v<Resource> && (!std::is_void_v<std::remove_pointer_t<Resource>>)
        {
            return *Get();
        }

        [[nodiscard]] constexpr Resource operator->() const noexcept
        requires std::is_pointer_v<Resource>
        {
            return m_resource.Get();
        }

        [[nodiscard]] constexpr const Deleter& GetDeleter() const noexcept
        {
            return m_deleter.Get();
        }

    private:
        [[no_unique_address]] Resource1 m_resource;
        // [[no_unique_address]] Resource1 m_resource{};
        [[no_unique_address]] Wrap<Deleter> m_deleter{};
        bool m_execOnReset = false;

#ifndef _MSC_VER
        //BUG: This produces error C2382 on MSVC <= 17.8.3 :C
        template<typename Resource2, typename Deleter2, typename Sentinel>
        friend constexpr UniqueResource<std::decay_t<Resource2>, std::decay_t<Deleter2>> MakeUniqueResourceChecked(Resource2&&, const Sentinel&, Deleter2&&)
        noexcept(std::is_nothrow_constructible_v<std::decay_t<Resource2>, Resource2> && std::is_nothrow_constructible_v<std::decay_t<Deleter2>, Deleter2>);
#else
        friend struct ::TRAP::INTERNAL::MSVCFriendNoexceptWorkaroundUniqueResource;
#endif

        template<typename Resource2, typename Deleter2>
        constexpr UniqueResource(Resource2&& r, Deleter2&& d, Dummy noop) noexcept(std::is_nothrow_constructible_v<Resource, Resource2> &&
                                                                                   std::is_nothrow_constructible_v<Deleter, Deleter2>)
            : m_resource(std::forward<Resource2>(r), noop),
              m_deleter(std::forward<Deleter>(d), noop)
        {
        }
    };

    template<typename Resource, typename Deleter>
    UniqueResource(Resource, Deleter) -> UniqueResource<Resource, Deleter>;

#ifndef _MSC_VER
    template<typename Resource, typename Deleter, typename Sentinel = std::decay_t<Resource>>
    [[nodiscard]] constexpr UniqueResource<std::decay_t<Resource>, std::decay_t<Deleter>> MakeUniqueResourceChecked(Resource&& r, const Sentinel& invalid, Deleter&& d)
    noexcept(std::is_nothrow_constructible_v<std::decay_t<Resource>, Resource> && std::is_nothrow_constructible_v<std::decay_t<Deleter>, Deleter>)
    {
        if(r == invalid)
            return {std::forward<Resource>(r), std::forward<Deleter>(d), {}};

        return {std::forward<Resource>(r), std::forward<Deleter>(d)};
    }
#else
    namespace INTERNAL
    {
        struct MSVCFriendNoexceptWorkaroundUniqueResource
        {
            template<typename Resource, typename Deleter, typename Sentinel = std::decay_t<Resource>>
            [[nodiscard]] static constexpr UniqueResource<std::decay_t<Resource>, std::decay_t<Deleter>> MakeUniqueResourceChecked(Resource&& r, const Sentinel& invalid, Deleter&& d)
            noexcept(std::is_nothrow_constructible_v<std::decay_t<Resource>, Resource> && std::is_nothrow_constructible_v<std::decay_t<Deleter>, Deleter>)
            {
                if(r == invalid)
                    return {std::forward<Resource>(r), std::forward<Deleter>(d), {}};

                return {std::forward<Resource>(r), std::forward<Deleter>(d)};
            }
        };
    }

    template<typename Resource, typename Deleter, typename Sentinel = std::decay_t<Resource>>
    [[nodiscard]] constexpr auto MakeUniqueResourceChecked(Resource&& r, const Sentinel& invalid, Deleter&& d)
    noexcept(noexcept(INTERNAL::MSVCFriendNoexceptWorkaroundUniqueResource::MakeUniqueResourceChecked(r, invalid, d)))
    -> decltype(INTERNAL::MSVCFriendNoexceptWorkaroundUniqueResource::MakeUniqueResourceChecked(r, invalid, d))
    {
        return INTERNAL::MSVCFriendNoexceptWorkaroundUniqueResource::MakeUniqueResourceChecked(r, invalid, d);
    }
#endif
}

#endif /*TRAP_UNIQUE_RESOURCE_H*/
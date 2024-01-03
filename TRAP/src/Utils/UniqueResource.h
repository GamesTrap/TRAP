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

    /// @brief General-purpose scope guard intended to call its exit function
    ///        when a scope is exited via an exception.
    ///
    ///        A ScopeFail may be either active, i.e. calls its exit function on destruction,
    ///        or inactive, i.e. does nothing on destruction.
    ///        A ScopeFail is active after constructed from an exit function.
    ///
    ///        A ScopeFail can become inactive by calling Release() on it either manually or automatically
    ///        (by the move constructor). An inactive ScopeFail may also be obtained by initializing
    ///        with another inactive ScopeFail. Once a ScopeFail is inactive, it cannot become active again.
    ///
    ///        A ScopeFail effectively holds an EF and a bool flag indicating if it is active, along with a
    ///        counter of uncaught exceptions used for detecting whether the destructor is called during
    ///        stack unwinding.
    /// @tparam EF Type of stored exit function.
    template<typename EF>
    class ScopeFail
    {
    public:
        /// @brief Creates a ScopeFail from a function, a function object or another ScopeFail.
        ///        Initializes the exit function with a function or function object, and initializes
        ///        the counter of uncaught exceptions as if with std::uncaught_exceptions().
        ///        The constructed ScopeFail is active.
        ///
        ///        The stored EF is initialized with f.
        ///        The behavior is undefined if calling f throws an exception or results in
        ///        undefined behavior, even if f has not been called.
        /// @param f Function or function object used for initializing the stored EF.
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

        /// @brief Creates a ScopeFail from a function, a function object or another ScopeFail.
        ///        Initializes the exit function with a function or function object, and initializes
        ///        the counter of uncaught exceptions as if with std::uncaught_exceptions().
        ///        The constructed ScopeFail is active.
        ///
        ///        The stored EF is initialized with std::forward<EFPtr>(f).
        ///        The behavior is undefined if calling f throws an exception or results in
        ///        undefined behavior, even if f has not been called.
        /// @param f Function or function object used for initializing the stored EF.
        template<typename EFPtr>
        requires INTERNAL::NotSameAs<std::remove_cvref_t<EFPtr>, ScopeFail> && std::constructible_from<EF, EFPtr> &&
                 INTERNAL::NotLValueReference<EFPtr> && std::is_nothrow_constructible_v<EF, EFPtr>
        constexpr explicit ScopeFail(EFPtr&& f) noexcept
            : m_exitFunction(std::forward<EFPtr>(f))
        {
        }

        /// @brief Creates a ScopeFail from a function, a function object or another ScopeFail.
        ///        Move constructor.
        ///        Initializes the stored EF with the one in rhs, and initializes the counter
        ///        of uncaught exceptions with the one in rhs. The constructed ScopeFail is active
        ///        if and only if rhs is active before the construction.
        ///
        ///        Initializes stored EF (denoted by exitFunction) with std::forward<EF>(rhs.exitFunction).
        ///        After successful move construction, rhs.Release() is called and other becomes inactive.
        /// @param rhs ScopeFail to move from.
        constexpr ScopeFail(ScopeFail&& rhs) noexcept
        requires std::is_nothrow_move_constructible_v<EF>
            : m_exitFunction(std::forward<EF>(rhs.m_exitFunction))
        {
            rhs.Release();
        }

        /// @brief Creates a ScopeFail from a function, a function object or another ScopeFail.
        ///        Move constructor.
        ///        Initializes the stored EF with the one in rhs, and initializes the counter
        ///        of uncaught exceptions with the one in rhs. The constructed ScopeFail is active
        ///        if and only if rhs is active before the construction.
        ///
        ///        Initializes stored EF (denoted by exitFunction) with rhs.exitFunction.
        ///        After successful move construction, rhs.Release() is called and other becomes inactive.
        /// @param rhs ScopeFail to move from.
        constexpr ScopeFail(ScopeFail&& rhs) noexcept(std::is_nothrow_copy_constructible_v<EF>)
        requires (!std::is_nothrow_move_constructible_v<EF>) && std::is_copy_constructible_v<EF>
            : m_exitFunction(rhs.m_exitFunction)
        {
            rhs.Release();
        }

        /// @brief ScopeFail is not copy constructible.
        consteval ScopeFail(const ScopeFail&) noexcept = delete;
        /// @brief ScopeFail is not assignable.
        consteval ScopeFail& operator=(const ScopeFail&) noexcept = delete;
        /// @brief ScopeFail is not assignable.
        consteval ScopeFail& operator=(ScopeFail&&) noexcept = delete;

        /// @brief Calls the exit function if the result of std::uncaught_exceptions() is greater
        ///        than the counter of uncaught exceptions (typically on stack unwinding) and the
        ///        ScopeFail is active, then destroys the stored EF (if it is a function object) and
        ///        any other non-static data members.
        /// @note Whether the destructor is called on stack unwinding can be detected by the comparison
        ///       of the result of std::uncaught_exceptions() and the counter of uncaught exceptions in
        ///       the ScopeFail.
        ~ScopeFail() noexcept(noexcept(this->m_exitFunction))
        {
            if(std::uncaught_exceptions() > m_uncaughtInit)
                m_exitFunction();
        }

        /// @brief Makes the ScopeFail inactive.
        ///        Once a ScopeFail is inactive, it cannot become active again,
        ///        and will not call its exit function on destruction.
        /// @note Release may be either manually called or automatically called by
        ///       ScopeFail's move constructor.
        constexpr void Release() noexcept
        {
            m_uncaughtInit = std::numeric_limits<i32>::max();
        }

    private:
        [[no_unique_address]] EF m_exitFunction;
        i32 m_uncaughtInit = std::uncaught_exceptions();
    };

    /// @brief One deduction guide is provided for ScopeFail to permit
    ///        deduction from an argument of function or function object type.
    ///        The argument (after function-to-pointer decay, if any) is copied or moved into
    ///        the constructed ScopeFail.
    template<typename EF>
    ScopeFail(EF) -> ScopeFail<EF>;

    /// @brief UniqueResource is a universal RAII wrapper for resource handles that
    ///        owns and manages a resource through a handle and disposes of that
    ///        resource when the UniqueResource is destroyed.
    ///
    ///        The resource is disposed of using the deleter of type Deleter when
    ///        either of the following happens:
    ///            - The managing UniqueResource object is destroyed,
    ///            - The managing UniqueResource object is assigned from another resource via operator= or Reset().
    ///        Let type Resource be R if R is an object type, or std::reference_wrapper<std::remove_reference_t<R>> otherwise:
    ///            - UniqueResource effectively holds a subobject of type Resource which is or wraps the resource handle,
    ///              a deleter of type Deleter and a bool flag indicating whether the wrapper is owning the resource.
    ///            - For explanatory purpose, the subobject of type Resource is called stored resource handle, and the
    ///              stored (if R is an object type) or wrapped (if R is a reference type) R is called underlying
    ///              resource handle.
    /// @tparam Resource Resource handle type.
    /// @tparam Deleter Deleter type.
    /// @note Resource handle types satisfying NullablePointer can also be managed by std::unique_ptr.
    ///       Unlike std::unique_ptr, UniqueResource does not require NullablePointer.
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
        /// @brief Default constructor.
        ///        Value-initializes the stored resource handle and the deleter.
        ///        The constructed UniqueResource does not own the resource.
        constexpr UniqueResource() = default;

        /// @brief The stored resource handle is initialized with std::forward<Resource2>(r) if
        ///        std::is_nothrow_constructible_v<Resource1, Resource2> is true, otherwise r.
        ///        If initialization of the stored resource handle throws an exception, calls d(r).
        ///        Then, the deleter is initialized with std::forward<Deleter2>(d) if
        ///        std::is_nothrow_constructible_v<Deleter, Deleter2> is true, otherwise d.
        ///        If initialization of deleter throws an exception, calls d(res).
        ///        The constructed UniqueResource owns the resource.
        /// @param r A resource handle.
        /// @param d A deleter to use to dispose the resource.
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

        /// @brief Move constructor.
        ///        The stored resource handle is initialized from the one of rhs, using std::move
        ///        if std::is_nothrow_move_constructible_v<Resource1> is true. If initialization
        ///        of the stored resource handle throws an exception, rhs is not modified.
        ///        Then, the deleter is initialized with the one of rhs, using std::move if
        ///        std::is_nothrow_move_constructible_v<Deleter> is true. If initialization
        ///        of the deleter throws an exception and std::is_nothrow_move_constructible_v<Resource1> is true
        ///        and rhs owns the resource, calls the deleter of rhs with res to dispose the resource,
        ///        then calls rhs.Release().
        ///        After construction, the constructed UniqueResource owns its resource if and only
        ///        if rhs owned the resource before the construction, and rhs is set to not own the resource.
        /// @param rhs Another UniqueResource to acquire the ownership from.
        constexpr UniqueResource(UniqueResource&& rhs) noexcept
        requires std::is_nothrow_move_constructible_v<Resource1> && std::is_nothrow_move_constructible_v<Deleter>
            : m_resource(std::move(rhs.m_resource)),
              m_deleter(std::move(rhs.m_deleter)),
              m_execOnReset(std::exchange(rhs.m_execOnReset, false))
        {
        }

        /// @brief Move constructor.
        ///        The stored resource handle is initialized from the one of rhs, using std::move
        ///        if std::is_nothrow_move_constructible_v<Resource1> is true. If initialization
        ///        of the stored resource handle throws an exception, rhs is not modified.
        ///        Then, the deleter is initialized with the one of rhs, using std::move if
        ///        std::is_nothrow_move_constructible_v<Deleter> is true. If initialization
        ///        of the deleter throws an exception and std::is_nothrow_move_constructible_v<Resource1> is true
        ///        and rhs owns the resource, calls the deleter of rhs with res to dispose the resource,
        ///        then calls rhs.Release().
        ///        After construction, the constructed UniqueResource owns its resource if and only
        ///        if rhs owned the resource before the construction, and rhs is set to not own the resource.
        /// @param rhs Another UniqueResource to acquire the ownership from.
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

        /// @brief Move constructor.
        ///        The stored resource handle is initialized from the one of rhs, using std::move
        ///        if std::is_nothrow_move_constructible_v<Resource1> is true. If initialization
        ///        of the stored resource handle throws an exception, rhs is not modified.
        ///        Then, the deleter is initialized with the one of rhs, using std::move if
        ///        std::is_nothrow_move_constructible_v<Deleter> is true. If initialization
        ///        of the deleter throws an exception and std::is_nothrow_move_constructible_v<Resource1> is true
        ///        and rhs owns the resource, calls the deleter of rhs with res to dispose the resource,
        ///        then calls rhs.Release().
        ///        After construction, the constructed UniqueResource owns its resource if and only
        ///        if rhs owned the resource before the construction, and rhs is set to not own the resource.
        /// @param rhs Another UniqueResource to acquire the ownership from.
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

        /// @brief Disposes the resource by calling the deleter with the underlying
        ///        resource handle if the UniqueResource owns it, equivalent to
        ///        calling Reset(). Then destroys the stored resource handle and the deleter.
        constexpr ~UniqueResource()
        {
            Reset();
        }

        /// @brief Move assignment operator. Replaces the managed resource
        ///        and the deleter with rhs's.
        ///        Formally, let Resource1 be the type of stored resource handle.
        ///            - First, calls Reset() to dispose the currently owned resource, if any.
        ///            - Then assigns the stored resource handle and the deleter with rhs's.
        ///              std::move is applied to the stored resource handle or the deleter of
        ///              rhs if std::is_nothrow_move_assignable_v<Resource1> or
        ///              std::is_nothrow_move_assignable_v<Deleter> is true respectively.
        ///              Assignment of the stored resource handle is executed first, unless
        ///              std::is_nothrow_move_assignable_v<Deleter> is false and
        ///              std::is_nothrow_move_assignable_v<Resource1> is true.
        ///            - Finally, sets *this to own the resource if and only if rhs
        ///              owned it before assignment, and rhs not to own the resource.
        /// @param rhs Resource wrapper from which ownership will be transferred.
        /// @return *this.
        /// @note If a copy of a member throws an exception, this mechanism leaves rhs intact
        ///       and *this in the released state.
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

        /// @brief Disposes the resource by calling the deleter with the underlying
        ///        resource handle if the UniqueResource owns it.
        ///        The UniqueResource does not own the resource after the call.
        constexpr void Reset() noexcept
        {
            if(m_execOnReset)
            {
                m_execOnReset = false;
                m_deleter.Get()(m_resource.Get());
            }
        }

        /// @brief Replaces the resource by calling Reset() and then assigns the stored
        ///        resource handle with std::forward<Resource2>(r) if
        ///        std::is_nothrow_assignable_v<Resource1, Resource2> is true, otherwise
        ///        std::as_const(r), where Resource1 is the type of stored resource handle.
        ///        The UniqueResource owns the resource after the call.
        ///
        ///        If copy-assignment of the store resource handle throws an exception,
        ///        calls del(r), where del is the deleter object.
        /// @param r Resource handle for a new resource to manage.
        template<typename Resource2>
        constexpr void Reset(Resource2&& r)
        {
            Reset();
            if constexpr(std::is_nothrow_assignable_v<Resource1&, Resource2>)
                m_resource.Get() = std::forward<Resource2>(r);
            else
                m_resource.Get() = std::as_const(r);
            m_execOnReset = true;
        }

        /// @brief Releases the ownership of the managed resource if any.
        ///        The destructor will not execute the deleter after the call,
        ///        unless Reset() is called later for managing a new resource.
        /// @note Unlike std::unique_ptr::release(), this function is not
        ///       required to modify the stored resource handle.
        constexpr void Release() noexcept
        {
            m_execOnReset = false;
        }

        /// @brief Accesses the underlying resource handle.
        /// @return The underlying resource handle.
        [[nodiscard]] constexpr const Resource& Get() const noexcept
        {
            return m_resource.Get();
        }

        /// @brief Access the object or function pointed by the underlying resource handle which is a pointer.
        ///        If the resource handle is not pointing to an object or a function, the behavior is undefined.
        /// @return The object or function pointed by the underlying resource handle.
        [[nodiscard]] constexpr std::add_lvalue_reference_t<std::remove_pointer_t<Resource>> operator*() const noexcept
        requires std::is_pointer_v<Resource> && (!std::is_void_v<std::remove_pointer_t<Resource>>)
        {
            return *Get();
        }

        /// @brief Get a copy of the underlying resource handle which is a pointer.
        ///        The return value is typically used to access the pointed object.
        /// @return Copy of the underlying resource handle.
        [[nodiscard]] constexpr Resource operator->() const noexcept
        requires std::is_pointer_v<Resource>
        {
            return m_resource.Get();
        }

        /// @brief Accesses the deleter object which would be used for disposing
        ///        the managed resource.
        /// @return The stored deleter.
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

    /// @brief One deduction guide is provided for UniqueResource to permit deduction
    ///        from given resource handle and deleter.
    ///        The arguments (after array-to-pointer and function-to-pointer decay, if any)
    ///        are copied or moved into the constructed UniqueResource.
    template<typename Resource, typename Deleter>
    UniqueResource(Resource, Deleter) -> UniqueResource<Resource, Deleter>;

#ifndef _MSC_VER
    /// @brief Creates a UniqueResource, its stored resource handle is initialized with
    ///        std::forward<Resource>(r) and its deleter with std::forward<Deleter>(d).
    ///        The created UniqueResource owns the resource if and only if bool(r == invalid) is false.
    /// @param r A resource handle.
    /// @param invalid A value indicating the resource handle is invalid.
    /// @param d A deleter to use to dispose the resource.
    /// @return A UniqueResource as described above.
    /// @note MakeUniqueResourceChecked() exists to avoid calling a deleter function with an invalid argument.
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
            /// @brief Creates a UniqueResource, its stored resource handle is initialized with
            ///        std::forward<Resource>(r) and its deleter with std::forward<Deleter>(d).
            ///        The created UniqueResource owns the resource if and only if bool(r == invalid) is false.
            /// @param r A resource handle.
            /// @param invalid A value indicating the resource handle is invalid.
            /// @param d A deleter to use to dispose the resource.
            /// @return A UniqueResource as described above.
            /// @note MakeUniqueResourceChecked() exists to avoid calling a deleter function with an invalid argument.
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

    /// @brief Creates a UniqueResource, its stored resource handle is initialized with
    ///        std::forward<Resource>(r) and its deleter with std::forward<Deleter>(d).
    ///        The created UniqueResource owns the resource if and only if bool(r == invalid) is false.
    /// @param r A resource handle.
    /// @param invalid A value indicating the resource handle is invalid.
    /// @param d A deleter to use to dispose the resource.
    /// @return A UniqueResource as described above.
    /// @note MakeUniqueResourceChecked() exists to avoid calling a deleter function with an invalid argument.
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
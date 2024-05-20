/// Optional - An implementation of std::optional with additional extensions

/// Implemented P2988R4 - TRAP::Optional<T&> with rebinding assignment
/// Implemented C++23 monadic functions for C++20

#ifndef TRAP_OPTIONAL_H
#define TRAP_OPTIONAL_H

#include <exception>
#include <functional>
#include <initializer_list>
#include <memory>

#if !defined(__EXCEPTIONS) && !defined(_HAS_EXCEPTIONS) && !defined(__cpp_exceptions)
    #define TRAP_OPTIONAL_NO_EXCEPTIONS
#endif

#ifdef _MSC_VER
    #pragma warning( push )
    #pragma warning( disable: 4582 4583 )
#endif /*_MSC_VER*/

namespace TRAP
{
    template<typename T>
    class Optional;

    struct BadOptionalAccess final : std::exception
    {
        [[nodiscard]] constexpr const char* what() const noexcept override
        {
            return "BadOptionalAccess in TRAP::Optional";
        }
    };

    namespace INTERNAL
    {
        template<typename E>
        [[noreturn]] constexpr void DoThrow(auto&&... args)
        {
#ifdef TRAP_OPTIONAL_NO_EXCEPTIONS
            std::abort();
#else
            throw E{std::forward<decltype(args)>(args)...};
#endif
        }

        struct IAmSecret{};

        template<typename T>
        concept IsMoveAssignableAndConstructible = (std::is_move_assignable_v<T> && std::is_move_constructible_v<T>);

        template<typename T>
        concept IsTriviallyMoveAssignableAndConstructible = (std::is_trivially_move_assignable_v<T> && std::is_trivially_move_constructible_v<T>);

        template<typename T>
        concept IsTriviallyCopyAssignableAndConstructible = (std::is_trivially_copy_assignable_v<T> && std::is_trivially_copy_constructible_v<T>);

        template<typename T>
        concept IsCopyAssignableAndConstructible = (std::is_copy_assignable_v<T> && std::is_copy_constructible_v<T>);

        template<typename T, typename... Args>
        concept IsConstructibleFromOne = (std::is_constructible_v<T, Args> || ...);

        template<typename T, typename... Args>
        concept IsConvertibleFromOne = (std::is_convertible_v<Args, T> || ...);

        template<typename T, typename... Args>
        concept IsAssignableFromOne = (std::is_assignable_v<T, Args> || ...);

        template<typename T, typename X, typename U>
        concept OptionalConstructorRequirement = (std::is_constructible_v<T, X> &&
                                                  !INTERNAL::IsConstructibleFromOne<T, Optional<U>&, const Optional<U>&, Optional<U>&&, const Optional<U>&&> &&
                                                  !INTERNAL::IsConvertibleFromOne<T, Optional<U>&, const Optional<U>&, Optional<U>&&, const Optional<U>&&>);

        template<typename T, typename X, typename U>
        concept OptionalAssignmentRequirement = (std::is_assignable_v<T&, X> &&
                                                 INTERNAL::OptionalConstructorRequirement<T, X, U> &&
                                                 !INTERNAL::IsAssignableFromOne<T, Optional<U>&, Optional<U>&&, const Optional<U>&, const Optional<U>&&>);
    }

    struct NullOptT
    {
        constexpr explicit NullOptT(int)
        {
        }
    };
    struct InPlaceT
    {
        constexpr explicit InPlaceT() = default;
    };

    inline constexpr NullOptT NullOpt{0};
    inline constexpr InPlaceT InPlace;

    template<typename T>
    class Optional
    {
        constexpr void MaybeInitUnion()
        {
            if(std::is_constant_evaluated())
            {
                m_dummy = 0;
            }
        }

        template<typename X>
        struct IsOptionalImpl : std::false_type
        {};

        template<typename X>
        struct IsOptionalImpl<Optional<X>> : std::true_type{};

        template<typename X>
        using IsOptional = IsOptionalImpl<std::decay_t<X>>;

    public:
        using value_type = T;

        //-------------------------------------------------------------------------------------------------------------------//
        //Constructors-------------------------------------------------------------------------------------------------------//
        //-------------------------------------------------------------------------------------------------------------------//

        constexpr Optional() noexcept
            : Optional(NullOpt)
        {
        }

        constexpr Optional(NullOptT) noexcept
        {
        }

        constexpr Optional(const Optional& other)
        requires std::is_trivially_copy_constructible_v<T> = default;

        constexpr Optional(const Optional& other)
        requires (std::is_copy_constructible_v<T> && !std::is_trivially_copy_constructible_v<T>)
            : m_active(other.m_active)
        {
            ConstructFromOptional(other);
        }

        constexpr Optional(Optional&& other)
        requires std::is_trivially_move_constructible_v<T> = default;

        constexpr Optional(Optional&& other) noexcept(std::is_nothrow_move_constructible_v<T>)
        requires (std::is_move_constructible_v<T> && !std::is_trivially_move_constructible_v<T>)
            : m_active(other.m_active)
        {
            ConstructFromOptional(std::forward<Optional>(other));
        }

        template<typename U = T>
        constexpr explicit(!std::is_convertible_v<U, T>) Optional(U&& u)
        requires (std::is_constructible_v<T, U> &&
                  !std::is_same_v<std::remove_cvref_t<U>, InPlaceT> &&
                  !std::is_same_v<std::remove_cvref_t<U>, Optional>)
            : m_active(true)
        {
            ConstructFrom(std::forward<U>(u));
        }

        template<typename... Args>
        constexpr explicit Optional(InPlaceT, Args&&... args)
        requires std::is_constructible_v<T, Args...>
            : m_active(true)
        {
            ConstructFrom(std::forward<Args>(args)...);
        }

        template<typename U, typename... Args>
        constexpr explicit Optional(InPlaceT, std::initializer_list<U> il, Args&&... args)
        requires std::is_constructible_v<T, std::initializer_list<U>&, Args...>
            : m_active(true)
        {
            ConstructFrom(il, std::forward<Args>(args)...);
        }

        template<typename U = T>
        constexpr explicit(!std::is_convertible_v<const U&, T>) Optional(const Optional<U>& other)
        requires INTERNAL::OptionalConstructorRequirement<T, const U&, U>
            : m_active(other.HasValue())
        {
            ConstructFromOptional(other);
        }

        template<typename U = T>
        constexpr explicit(!std::is_convertible_v<U, T>) Optional(Optional<U>&& other)
        requires INTERNAL::OptionalConstructorRequirement<T, U&&, U>
            : m_active(other.HasValue())
        {
            ConstructFromOptional(std::forward<Optional<U>>(other));
        }

        //-------------------------------------------------------------------------------------------------------------------//
        //Destructors--------------------------------------------------------------------------------------------------------//
        //-------------------------------------------------------------------------------------------------------------------//

        constexpr ~Optional()
        requires std::is_trivially_destructible_v<T> = default;

        constexpr ~Optional()
        requires (!std::is_trivially_destructible_v<T>)
        {
            if(!m_active)
                return;

            m_value.~T();
        }

        //-------------------------------------------------------------------------------------------------------------------//
        //Assignment operators-----------------------------------------------------------------------------------------------//
        //-------------------------------------------------------------------------------------------------------------------//

        constexpr Optional& operator=(NullOptT) noexcept
        {
            if(!m_active)
                return *this;

            ResetNoCheck();
            return *this;
        }

        constexpr Optional& operator=(const Optional& other)
        requires INTERNAL::IsTriviallyCopyAssignableAndConstructible<T>
        = default;

        constexpr Optional& operator=(const Optional& other)
        requires (INTERNAL::IsCopyAssignableAndConstructible<T> && !INTERNAL::IsTriviallyCopyAssignableAndConstructible<T>)
        {
            return AssignFromOptional(other);
        }

        constexpr Optional& operator=(Optional&& other)
        requires INTERNAL::IsTriviallyMoveAssignableAndConstructible<T>
        = default;

        constexpr Optional& operator=(Optional&& other)
        requires (INTERNAL::IsMoveAssignableAndConstructible<T> && !INTERNAL::IsTriviallyMoveAssignableAndConstructible<T>)
        {
            return AssignFromOptional(std::forward<Optional>(other));
        }

        template<typename U = T>
        constexpr Optional& operator=(U&& u)
        requires (!std::is_same_v<std::remove_cvref_t<U>, Optional> &&
                !(std::is_scalar_v<T> && std::is_same_v<T, std::decay_t<U>>) &&
                std::is_constructible_v<T, U> &&
                std::is_assignable_v<T&, U>)
        {
            if(m_active)
                **this = std::forward<U>(u);
            else
                ConstructFrom(std::forward<U>(u));

            return *this;
        }

        template<typename U>
        constexpr Optional& operator=(const Optional<U>& u)
        requires INTERNAL::OptionalAssignmentRequirement<T, const U&, U>
        {
            return AssignFromOptional(u);
        }

        template<typename U>
        constexpr Optional& operator=(Optional<U>&& u)
        requires INTERNAL::OptionalAssignmentRequirement<T, U, U>
        {
            return AssignFromOptional(std::forward<Optional<U>>(u));
        }

        //-------------------------------------------------------------------------------------------------------------------//
        //Observers----------------------------------------------------------------------------------------------------------//
        //-------------------------------------------------------------------------------------------------------------------//

        [[nodiscard]] constexpr const T* operator->() const noexcept
        {
            return Ptr();
        }

        [[nodiscard]] constexpr T* operator->() noexcept
        {
            return Ptr();
        }

        [[nodiscard]] constexpr const T& operator*() const& noexcept
        {
            return m_value;
        }

        [[nodiscard]] constexpr T& operator*() & noexcept
        {
            return m_value;
        }

        [[nodiscard]] constexpr const T&& operator*() const&& noexcept
        {
            return std::move(m_value);
        }

        [[nodiscard]] constexpr T&& operator*() && noexcept
        {
            return std::move(m_value);
        }

        [[nodiscard]] constexpr explicit operator bool() const noexcept
        {
            return m_active;
        }

        [[nodiscard]] constexpr bool HasValue() const noexcept
        {
            return m_active;
        }

        [[nodiscard]] constexpr T& Value() &
        {
            if (*this)
                return **this;

            INTERNAL::DoThrow<BadOptionalAccess>();
        }

        [[nodiscard]] constexpr const T& Value() const&
        {
            if(*this)
                return **this;

            INTERNAL::DoThrow<BadOptionalAccess>();
        }

        [[nodiscard]] constexpr T&& Value() &&
        {
            if(*this)
                return std::move(m_value);

            INTERNAL::DoThrow<BadOptionalAccess>();
        }

        [[nodiscard]] constexpr const T&& Value() const&&
        {
            if(*this)
                return std::move(m_value);

            INTERNAL::DoThrow<BadOptionalAccess>();
        }

        template<typename U>
        [[nodiscard]] constexpr T ValueOr(U&& u) const &
        requires (std::is_copy_constructible_v<T> && std::is_convertible_v<U&&, T>)
        {
            return bool(*this) ? **this : static_cast<T>(std::forward<U>(u));
        }

        template<typename U>
        [[nodiscard]] constexpr T ValueOr(U&& u) &&
        requires (std::is_move_constructible_v<T> && std::is_convertible_v<U&&, T>)
        {
            return bool(*this) ? std::move(m_value) : static_cast<T>(std::forward<U>(u));
        }

        //-------------------------------------------------------------------------------------------------------------------//
        //Modifiers----------------------------------------------------------------------------------------------------------//
        //-------------------------------------------------------------------------------------------------------------------//

        template<typename... Args>
        requires std::is_constructible_v<T, Args...>
        constexpr void Emplace(Args&&... args)
        {
            Reset();
            ConstructFrom(std::forward<Args>(args)...);
        }

        template<typename U, typename... Args>
        requires std::is_constructible_v<T, std::initializer_list<U>&, Args...>
        constexpr void Emplace(std::initializer_list<U> il, Args&&... args)
        {
            Reset();
            ConstructFrom(il, std::forward<Args>(args)...);
        }

        constexpr void Swap(Optional& other) noexcept(std::is_nothrow_move_constructible_v<T> && std::is_nothrow_swappable_v<T>)
        requires std::is_move_constructible_v<T>
        {
            switch(static_cast<char>(m_active) + static_cast<char>(other.m_active) * 2)
            {
            case 0:
                break;
            case 1:
                other.ConstructFrom(std::move(**this));
                ResetNoCheck();
                break;
            case 2:
                ConstructFrom(std::move(*other));
                other.ResetNoCheck();
                break;
            case 3:
                using std::swap;
                swap(**this, *other);
                break;
            }
        }

        constexpr void Reset() noexcept
        {
            if(!m_active)
                return;

            ResetNoCheck();
        }

        [[nodiscard]] constexpr Optional Take()
        {
            Optional ret = std::move(*this);
            Reset();
            return ret;
        }

        template<typename F, typename Ret = std::remove_cv_t<std::invoke_result_t<F, T&>>>
        constexpr auto Transform(F&& f) &
        requires (!std::is_void_v<Ret>)
        {
            if(m_active)
                return Optional<Ret>(std::invoke(std::forward<F>(f), **this));

            return Optional<Ret>(NullOpt);
        }

        template<typename F, typename Ret = std::remove_cv_t<std::invoke_result_t<F, const T&>>>
        requires (!std::is_void_v<Ret>)
        constexpr auto Transform(F&& f) const&
        {
            if(m_active)
                return Optional<Ret>(std::invoke(std::forward<F>(f), **this));

            return Optional<Ret>(NullOpt);
        }

        template<typename F, typename Ret = std::remove_cv_t<std::invoke_result_t<F, T>>>
        requires (!std::is_void_v<Ret>)
        constexpr auto Transform(F&& f) &&
        {
            if(m_active)
                return Optional<Ret>(std::invoke(std::forward<F>(f), std::move(**this)));

            return Optional<Ret>(NullOpt);
        }

        template<typename F, typename Ret = std::remove_cv_t<std::invoke_result_t<F, const T>>>
        requires (!std::is_void_v<Ret>)
        constexpr auto Transform(F&& f) const&&
        {
            if(m_active)
                return Optional<Ret>(std::invoke(std::forward<F>(f), std::move(**this)));

            return Optional<Ret>(NullOpt);
        }

        template<typename F>
        constexpr auto AndThen(F&& f) &
        {
            using Ret = std::invoke_result_t<F, T&>;
            static_assert(IsOptional<Ret>::value, "F must return an Optional");

            if(m_active)
                return std::invoke(std::forward<F>(f), **this);

            return std::remove_cvref_t<std::invoke_result_t<F, T&>>(NullOpt);
        }

        template<typename F>
        constexpr auto AndThen(F&& f) const&
        {
            using Ret = std::invoke_result_t<F, const T&>;
            static_assert(IsOptional<Ret>::value, "F must return an Optional");

            if(m_active)
                return std::invoke(std::forward<F>(f), **this);

            return std::remove_cvref_t<std::invoke_result_t<F, const T&>>(NullOpt);
        }

        template<typename F>
        constexpr auto AndThen(F&& f) &&
        {
            using Ret = std::invoke_result_t<F, T>;
            static_assert(IsOptional<Ret>::value, "F must return an Optional");

            if(m_active)
                return std::invoke(std::forward<F>(f), std::move(**this));

            return std::remove_cvref_t<std::invoke_result_t<F, T>>(NullOpt);
        }

        template<typename F>
        constexpr auto AndThen(F&& f) const&&
        {
            using Ret = std::invoke_result_t<F, const T>;
            static_assert(IsOptional<Ret>::value, "F must return an Optional");

            if(m_active)
                return std::invoke(std::forward<F>(f), std::move(**this));

            return std::remove_cvref_t<std::invoke_result_t<F, const T>>(NullOpt);
        }

        template<typename F>
        constexpr Optional<T> OrElse(F&& f) const&
        requires (std::copy_constructible<T> && std::invocable<F>)
        {
            using Ret = std::remove_cvref_t<std::invoke_result_t<F>>;
            static_assert(IsOptional<Ret>::value, "F must return an Optional");

            return *this ? *this : std::forward<F>(f)();

        }

        template<typename F>
        constexpr Optional<T> OrElse(F&& f) &&
        requires (std::move_constructible<T> && std::invocable<F>)
        {
            using Ret = std::remove_cvref_t<std::invoke_result_t<F>>;
            static_assert(IsOptional<Ret>::value, "F must return an Optional");

            return *this ? std::move(*this) : std::forward<F>(f)();
        }

        template<typename F>
        [[nodiscard]] constexpr T ValueOrElse(F&& f) const&
        requires (std::copy_constructible<T> && std::invocable<F>)
        {
            return m_active ? m_value : std::forward<F>(f)();
        }

        template<typename F>
        [[nodiscard]] constexpr T ValueOrElse(F&& f) &&
        requires (std::move_constructible<T> && std::invocable<F>)
        {
            return m_active ? std::move(**this) : std::forward<F>(f)();
        }

        template<typename F, typename U>
        [[nodiscard]] constexpr auto TransformOrElse(F&& f, U&& u) const&
        {
            return m_active ? std::invoke(std::forward<F>(f), **this) : std::forward<U>(u)();
        }

        template<typename F, typename U>
        [[nodiscard]] constexpr auto TransformOrElse(F&& f, U&& u) &&
        {
            return m_active ? std::invoke(std::forward<F>(f), std::move(**this)) : std::forward<U>(u)();
        }

        template<typename F, typename U>
        [[nodiscard]] constexpr U TransformOr(F&& f, U&& u) const&
        {
            return m_active ? std::invoke(std::forward<F>(f), **this) : std::forward<U>(u);
        }

        template<typename F, typename U>
        [[nodiscard]] constexpr U TransformOr(F&& f, U&& u) &&
        {
            return m_active ? std::invoke(std::forward<F>(f), std::move(**this)) : std::forward<U>(u);
        }

    private:
        template<typename Opt>
        constexpr auto& AssignFromOptional(Opt&& o)
        {
            switch(static_cast<char>(m_active) + static_cast<char>(o.HasValue()) * 2)
            {
            case 0:
                break;
            case 1:
                ResetNoCheck();
                break;
            case 2:
                ConstructFrom(*std::forward<Opt>(o));
                break;
            case 3:
                **this = *std::forward<Opt>(o);
                break;
            }

            return *this;
        }

        constexpr void ResetNoCheck()
        {
            if constexpr(!std::is_trivially_destructible_v<T>)
            {
                m_value.~T();
            }

            m_active = false;
        }

        template<typename... Args>
        constexpr void ConstructFrom(Args&&... args)
        {
            std::construct_at(std::addressof(m_value), static_cast<Args&&>(args)...);
            m_active = true;
        }

        template<typename Opt>
        constexpr void ConstructFromOptional(Opt&& opt)
        {
            if(m_active)
                ConstructFrom(*std::forward<Opt>(opt));
            else
                MaybeInitUnion();
        }

        [[nodiscard]] constexpr T* Ptr()
        {
            return std::addressof(m_value);
        }

        [[nodiscard]] constexpr const T* Ptr() const
        {
            return std::addressof(m_value);
        }

        union
        {
            char m_dummy = 0;
            T m_value;
        };
        bool m_active = false;
    };

	//-------------------------------------------------------------------------------------------------------------------//
	//Reference specialization (with rebinding)--------------------------------------------------------------------------//
	//-------------------------------------------------------------------------------------------------------------------//

    template<typename T>
    class Optional<T&>
    {
        template<typename X>
        struct IsOptionalImpl : std::false_type
        {};

        template<typename X>
        struct IsOptionalImpl<Optional<X>> : std::true_type{};

        template<typename X>
        using IsOptional = IsOptionalImpl<std::decay_t<X>>;

    public:
        using value_type = T&;

        //-------------------------------------------------------------------------------------------------------------------//
        //Constructors-------------------------------------------------------------------------------------------------------//
        //-------------------------------------------------------------------------------------------------------------------//

        constexpr Optional() noexcept = default;

        constexpr Optional(NullOptT) noexcept
        {
        }

        constexpr Optional(const Optional& other) noexcept = default;

        constexpr Optional(Optional&& other) noexcept = default;

        template<typename U = T>
        constexpr explicit(!std::is_convertible_v<U, T>) Optional(U&& u) noexcept
        requires (!IsOptional<std::decay_t<U>>::value)
            : m_value(std::addressof(u))
        {
            static_assert(std::is_constructible_v<std::add_lvalue_reference_t<T>, U>, "Must be able to bind U to T&");
        }

        template<typename U>
        constexpr explicit(!std::is_convertible_v<U, T>) Optional(const Optional<U>& rhs) noexcept
        {
            if(rhs.HasValue())
                m_value = std::addressof(*rhs);
        }

        //-------------------------------------------------------------------------------------------------------------------//
        //Destructors--------------------------------------------------------------------------------------------------------//
        //-------------------------------------------------------------------------------------------------------------------//

        constexpr ~Optional() = default;

        //-------------------------------------------------------------------------------------------------------------------//
        //Assignment operators-----------------------------------------------------------------------------------------------//
        //-------------------------------------------------------------------------------------------------------------------//

        constexpr Optional& operator=(NullOptT) noexcept
        {
            m_value = nullptr;

            return *this;
        }

        constexpr Optional& operator=(const Optional& rhs) noexcept = default;
        constexpr Optional& operator=(Optional&& rhs) noexcept = default;

        template<typename U = T>
        constexpr Optional& operator=(U&& u)
        requires (!IsOptional<std::decay_t<U>>::value)
        {
            static_assert(std::is_constructible_v<std::add_lvalue_reference_t<T>, U>, "Must be able to bind U to T&");

            m_value = std::addressof(u);

            return *this;
        }

        template<typename U>
        constexpr Optional& operator=(const Optional<U>& rhs) noexcept
        {
            static_assert(std::is_constructible_v<std::add_lvalue_reference_t<T>, U>, "Must be able to bind U to T&");

            m_value = std::addressof(rhs.Value());

            return *this;
        }

        //-------------------------------------------------------------------------------------------------------------------//
        //Observers----------------------------------------------------------------------------------------------------------//
        //-------------------------------------------------------------------------------------------------------------------//

        [[nodiscard]] constexpr const T* operator->() const noexcept
        {
            return m_value;
        }

        [[nodiscard]] constexpr T* operator->() noexcept
        {
            return m_value;
        }

        [[nodiscard]] constexpr T& operator*() noexcept
        {
            return *m_value;
        }

        [[nodiscard]] constexpr T& operator*() const noexcept
        {
            return *m_value;
        }

        [[nodiscard]] constexpr explicit operator bool() const noexcept
        {
            return m_value != nullptr;
        }

        [[nodiscard]] constexpr bool HasValue() const noexcept
        {
            return m_value != nullptr;
        }

        [[nodiscard]] constexpr T& Value()
        {
            if (*this)
                return **this;

            INTERNAL::DoThrow<BadOptionalAccess>();
        }

        [[nodiscard]] constexpr const T& Value() const
        {
            if (*this)
                return **this;

            INTERNAL::DoThrow<BadOptionalAccess>();
        }

        template<typename U>
        [[nodiscard]] constexpr T ValueOr(U&& u) const
        {
            static_assert(std::is_constructible_v<std::add_lvalue_reference_t<T>, decltype(u)>, "Must be able to bind U to T&");

            return bool(*this) ? **this : static_cast<T>(std::forward<U>(u));
        }

        //-------------------------------------------------------------------------------------------------------------------//
        //Modifiers----------------------------------------------------------------------------------------------------------//
        //-------------------------------------------------------------------------------------------------------------------//

        template<typename U>
        constexpr Optional& Emplace(U&& u) noexcept
        requires (!IsOptional<std::decay_t<U>>::value)
        {
            return *this = std::forward<U>(u);
        }

        constexpr void Swap(Optional& rhs) noexcept
        {
            std::swap(m_value, rhs.m_value);
        }


        constexpr void Reset() noexcept
        {
            m_value = nullptr;
        }

        [[nodiscard]] constexpr Optional Take()
        {
            Optional ret = std::move(*this);
            Reset();
            return ret;
        }

        template<typename F, typename Ret= std::remove_cv_t<std::invoke_result_t<F, T&>>>
        requires (!std::is_void_v<Ret>)
        constexpr auto Transform(F&& f) &
        {
            if(m_value)
                return Optional<Ret>(std::invoke(std::forward<F>(f), **this));

            return Optional<Ret>(NullOpt);
        }

        template<typename F, typename Ret = std::remove_cv_t<std::invoke_result_t<F, const T&>>>
        requires (!std::is_void_v<Ret>)
        constexpr auto Transform(F&& f) const&
        {
            if(m_value)
                return Optional<Ret>(std::invoke(std::forward<F>(f), **this));

            return Optional<Ret>(NullOpt);
        }

        template<typename F, typename Ret = std::remove_cv_t<std::invoke_result_t<F, T>>>
        requires (!std::is_void_v<Ret>)
        constexpr auto Transform(F&& f) &&
        {
            if(m_value)
                return Optional<Ret>(std::invoke(std::forward<F>(f), std::move(**this)));

            return Optional<Ret>(NullOpt);
        }

        template<typename F, typename Ret = std::remove_cv_t<std::invoke_result_t<F, const T>>>
        requires (!std::is_void_v<Ret>)
        constexpr auto Transform(F&& f) const&&
        {
            if(m_value)
                return Optional<Ret>(std::invoke(std::forward<F>(f), std::move(**this)));

            return Optional<Ret>(NullOpt);
        }

        template<typename F>
        constexpr auto AndThen(F&& f) &
        {
            using Ret = std::invoke_result_t<F, T&>;
            static_assert(IsOptional<Ret>::value, "F must return an Optional");

            if(m_value)
                return std::invoke(std::forward<F>(f), **this);

            return Ret(NullOpt);
        }

        template<typename F>
        constexpr auto AndThen(F&& f) const&
        {
            using Ret = std::invoke_result_t<F, const T&>;
            static_assert(IsOptional<Ret>::value, "F must return an Optional");

            if(m_value)
                return std::invoke(std::forward<F>(f), **this);

            return Ret(NullOpt);
        }

        template<typename F>
        constexpr auto AndThen(F&& f) &&
        {
            using Ret = std::invoke_result_t<F, T&>;
            static_assert(IsOptional<Ret>::value, "F must return an Optional");

            if(m_value)
                return std::invoke(std::forward<F>(f), **this);

            return Ret(NullOpt);
        }

        template<typename F>
        constexpr auto AndThen(F&& f) const&&
        {
            using Ret = std::invoke_result_t<F, const T&>;
            static_assert(IsOptional<Ret>::value, "F must return an Optional");

            if(m_value)
                return std::invoke(std::forward<F>(f), **this);

            return Ret(NullOpt);
        }

        template<typename F>
        constexpr Optional<T> OrElse(F&& f) const&
        requires (std::copy_constructible<T> && std::invocable<F>)
        {
            using Ret = std::remove_cv_t<std::invoke_result_t<F>>;
            static_assert(IsOptional<Ret>::value, "F must return an Optional");

            return *this ? *this : std::forward<F>(f)();

        }

        template<typename F>
        constexpr Optional<T> OrElse(F&& f) &&
        requires (std::move_constructible<T> && std::invocable<F>)
        {
            using Ret = std::remove_cv_t<std::invoke_result_t<F>>;
            static_assert(IsOptional<Ret>::value, "F must return an Optional");

            return *this ? std::move(*this) : std::forward<F>(f)();
        }

        template<typename F>
        [[nodiscard]] constexpr T ValueOrElse(F&& f) const&
        requires (std::copy_constructible<T> && std::invocable<F>)
        {
            return m_value ? **this : std::forward<F>(f)();
        }

        template<typename F>
        [[nodiscard]] constexpr T ValueOrElse(F&& f) &&
        requires (std::move_constructible<T> && std::invocable<F>)
        {
            return m_value ? std::move(**this) : std::forward<F>(f)();
        }

        template<typename F, typename U>
        [[nodiscard]] constexpr auto TransformOrElse(F&& f, U&& u) const&
        {
            return m_value ? std::invoke(std::forward<F>(f), **this) : std::forward<U>(u)();
        }

        template<typename F, typename U>
        [[nodiscard]] constexpr auto TransformOrElse(F&& f, U&& u) &&
        {
            return m_value ? std::invoke(std::forward<F>(f), std::move(**this)) : std::forward<U>(u)();
        }

        template<typename F, typename U>
        [[nodiscard]] constexpr U TransformOr(F&& f, U&& u) const&
        {
            return m_value ? std::invoke(std::forward<F>(f), **this) : std::forward<U>(u);
        }

        template<typename F, typename U>
        [[nodiscard]] constexpr U TransformOr(F&& f, U&& u) &&
        {
            return m_value ? std::invoke(std::forward<F>(f), std::move(**this)) : std::forward<U>(u);
        }

    private:

        T* m_value = nullptr;
    };

    template<typename T>
    requires (std::is_same_v<std::decay_t<T>, NullOptT> || std::is_same_v<std::decay_t<T>, InPlaceT>)
    class Optional<T>;

    template<typename T>
    Optional(T) -> Optional<T>;

    template<typename T>
    constexpr void swap(Optional<T>& x, Optional<T>& y) noexcept(noexcept(x.swap(y)))
    requires (std::is_move_constructible_v<T> && std::is_swappable_v<T>)
    {
        x.Swap(y);
    }

    template<typename T, typename U>
    [[nodiscard]] constexpr bool operator==(const Optional<T>& lhs, const Optional<U>& rhs)
    requires requires {bool(*lhs == *rhs);}
    {
        if(lhs.HasValue() != rhs.HasValue())
            return false;

        if(!lhs.HasValue())
            return true;

        return *lhs == *rhs;
    }

    template<typename T, typename U>
    [[nodiscard]] constexpr bool operator!=(const Optional<T>& lhs, const Optional<U>& rhs)
    requires requires { bool(*lhs != *rhs); }
    {
        if(lhs.HasValue() != rhs.HasValue())
            return true;

        if(!lhs)
            return false;

        return *lhs != *rhs;
    }

    template<typename T, typename U>
    [[nodiscard]] constexpr bool operator<(const Optional<T>& lhs, const Optional<U>& rhs)
    requires requires { bool(*lhs < *rhs); }
    {
        if(!lhs)
            return true;

        if(!rhs)
            return false;

        return *lhs < *rhs;
    }

    template<typename T, typename U>
    [[nodiscard]] constexpr bool operator>(const Optional<T>& lhs, const Optional<U>& rhs)
    requires requires { bool(*lhs > *rhs); }
    {
        if(!lhs)
            return false;

        if(!rhs)
            return true;

        return *lhs > *rhs;
    }

    template<typename T, typename U>
    [[nodiscard]] constexpr bool operator<=(const Optional<T>& lhs, const Optional<U>& rhs)
    requires requires { bool(*lhs <= *rhs); }
    {
        if(!lhs)
            return true;

        if(!rhs)
            return false;

        return *lhs <= *rhs;
    }

    template<typename T, typename U>
    [[nodiscard]] constexpr bool operator>=(const Optional<T>& lhs, const Optional<U>& rhs)
    requires requires { bool(*lhs >= *rhs); }
    {
        if(!lhs)
            return false;

        if(!rhs)
            return true;

        return *lhs >= *rhs;
    }

    template<typename T>
    [[nodiscard]] constexpr bool operator==(const Optional<T>& x, NullOptT) noexcept
    {
        return !x;
    }

    template<typename T>
    [[nodiscard]] constexpr std::strong_ordering operator<=>(const Optional<T>& x, NullOptT) noexcept
    {
        return bool(x) <=> false;
    }

#define MAKE_OP(OP, DEFAULT1, DEFAULT2) \
    template<typename T, typename U> \
    [[nodiscard]] constexpr bool operator OP (const Optional<T>& lhs, const U& rhs) \
    requires requires { bool(*lhs OP rhs); } \
    { \
        return bool(lhs) ? *lhs OP rhs : DEFAULT1; \
    } \
    \
    template<typename T, typename U> \
    [[nodiscard]] constexpr bool operator OP (const T& lhs, const Optional<U>& rhs) \
    requires requires { bool(lhs OP *rhs); } \
    { \
        return bool(rhs) ? lhs OP *rhs : DEFAULT2; \
    } \

    MAKE_OP(==, false, false);
    MAKE_OP(!=, true, true);
    MAKE_OP(<, true, false);
    MAKE_OP(>, false, true);
    MAKE_OP(<=, true, false);
    MAKE_OP(>=, false, true);

#undef MAKE_OP

    template<typename T = INTERNAL::IAmSecret, typename U, typename Ret = std::conditional_t<std::is_same_v<T, INTERNAL::IAmSecret>, std::decay_t<U>, T>>
    [[nodiscard]] constexpr auto MakeOptional(U&& v)
    {
        return Optional<std::remove_cvref_t<Ret>>(std::forward<U>(v));
    }

    template<typename T, typename... Args>
    [[nodiscard]] constexpr auto MakeOptional(Args&&... args)
    {
        return Optional<std::remove_cvref_t<T>>{InPlace, std::forward<Args>(args)...};
    }

    template<typename T, typename U, typename... Args>
    [[nodiscard]] constexpr auto MakeOptional(std::initializer_list<U> il, Args&&... args)
    {
        return Optional<std::remove_cvref_t<T>>{InPlace, il, std::forward<Args>(args)...};
    }
}

namespace std
{
    template<typename T>
    requires requires {std::hash<std::decay_t<T>>{};}
    struct hash<TRAP::Optional<T>>
    {
        [[nodiscard]] constexpr std::size_t operator()(const TRAP::Optional<T>& o) const
        {
            if(!o)
                return -1;

            return std::hash<std::decay_t<T>>{}(*o);
        }
    };
}

#ifdef _MSC_VER
    #pragma warning( pop )
#endif /*_MSC_VER*/

#endif /*TRAP_OPTIONAL_H*/

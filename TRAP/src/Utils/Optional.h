/// Optional - An implementation of std::optional with extensions
/// Originally written in 2017 by Sy Brand (tartanllama@gmail.com, @TartanLlama)
/// Modified by Jan "GamesTrap" Schuerkamp

#ifndef TRAP_OPTIONAL_H
#define TRAP_OPTIONAL_H

#include <exception>
#include <functional>
#include <new>
#include <type_traits>
#include <utility>
#include <variant>

#if defined(__EXCEPTIONS) || defined(_HAS_EXCEPTIONS) || defined(__cpp_exceptions)
    #define TRAP_OPTIONAL_THROW(e) throw e
#else
    #define TRAP_OPTIONAL_THROW(e) std::abort()
#endif

#ifdef _MSC_VER
    #pragma warning( push )
    #pragma warning( disable: 4582 4583 )
#endif /*_MSC_VER*/

namespace TRAP
{
    template<typename T>
    class Optional;

    namespace INTERNAL
    {
        template<typename T>
        struct IsOptionalImpl : std::false_type
        {};
        template<typename T>
        struct IsOptionalImpl<Optional<T>> : std::true_type
        {};

        /// @brief Trait for checking if a type is of type TRAP::Optional.
        /// @tparam T Type to check.
        template<typename T>
        using IsOptional = IsOptionalImpl<std::decay_t<T>>;

        /// @brief Change type void to std::monostate.
        /// @tparam U Type to check.
        template<typename U>
        using FixupVoid = std::conditional_t<std::is_void_v<U>, std::monostate, U>;

        template<typename F, typename = void, typename... U>
        struct ReturnsVoidImpl;
        template<typename F, typename... U>
        struct ReturnsVoidImpl<F, std::void_t<std::invoke_result_t<F, U...>>, U...>
            : std::is_void<std::invoke_result_t<F, U...>>
        {};

        /// @brief Check whether invoking F for some Us returns void.
        template<typename F, typename... U>
        using ReturnsVoid = ReturnsVoidImpl<F, void, U...>;

        /// @brief Enable template if invoking T for some Us returns void.
        template<typename T, typename... U>
        using EnableIfRetVoid = std::enable_if_t<ReturnsVoid<T&&, U...>::value>;

        /// @brief Disable template if invoking T for some Us returns void.
        template<typename T, typename... U>
        using DisableIfRetVoid = std::enable_if_t<!ReturnsVoid<T&&, U...>::value>;

        template<typename T, typename U>
        using EnableForwardValue = std::enable_if_t<std::is_constructible_v<T, U&&> &&
                                   !std::is_same_v<std::decay_t<U>, std::in_place_t> &&
                                   !std::is_same_v<Optional<T>, std::decay_t<U>>>;

        template<typename T, typename U, typename Other>
        using EnableFromOther = std::enable_if_t<std::is_constructible_v<T, Other> &&
                                                 !std::is_constructible_v<T, Optional<U>&> &&
                                                 !std::is_constructible_v<T, Optional<U>&&> &&
                                                 !std::is_constructible_v<T, const Optional<U>&> &&
                                                 !std::is_constructible_v<T, const Optional<U>&&> &&
                                                 !std::is_convertible_v<Optional<U>&, T> &&
                                                 !std::is_convertible_v<Optional<U>&&, T> &&
                                                 !std::is_convertible_v<const Optional<U>&, T> &&
                                                 !std::is_convertible_v<const Optional<U>&&, T>>;

        template<typename T, typename U>
        using EnableAssignForward = std::enable_if_t<!std::is_same_v<Optional<T>, std::decay_t<U>> &&
                                                     !std::conjunction_v<std::is_scalar<T>, std::is_same<T, std::decay_t<U>>> &&
                                                     std::is_constructible_v<T, U> && std::is_assignable_v<T&, U>>;

        template<typename T, typename U, typename Other>
        using EnableAssignFromOther = std::enable_if_t<std::is_constructible_v<T, Other> &&
                                                       std::is_assignable_v<T&, Other> &&
                                                       !std::is_constructible_v<T, Optional<U>&> &&
                                                       !std::is_constructible_v<T, Optional<U>&&> &&
                                                       !std::is_constructible_v<T, const Optional<U>&> &&
                                                       !std::is_constructible_v<T, const Optional<U>&&> &&
                                                       !std::is_convertible_v<Optional<U>&, T> &&
                                                       !std::is_convertible_v<Optional<U>&&, T> &&
                                                       !std::is_convertible_v<const Optional<U>&, T> &&
                                                       !std::is_convertible_v<const Optional<U>&&, T> &&
                                                       !std::is_assignable_v<T&, Optional<U>&> &&
                                                       !std::is_assignable_v<T&, Optional<U>&&> &&
                                                       !std::is_assignable_v<T&, const Optional<U>&> &&
                                                       !std::is_assignable_v<T&, const Optional<U>&&>>;

        /// @brief The storage base manages the actual storage, and correctly propagates
        ///        trivial destruction from T.
        ///        This case is for when T is not trivially destructible.
        template<typename T, bool = std::is_trivially_destructible_v<T>>
        struct OptionalStorageBase
        {
            constexpr OptionalStorageBase() noexcept
                : m_dummy(), m_hasValue(false)
            {}

            template<typename... U>
            constexpr OptionalStorageBase([[maybe_unused]] std::in_place_t _, U&&... u)
                : m_value(std::forward<U>(u)...), m_hasValue(true)
            {}

            constexpr ~OptionalStorageBase()
            {
                if (m_hasValue)
                {
                    std::destroy_at(std::addressof(m_value));
                    m_hasValue = false;
                }
            }

            struct Dummy {};
            union
            {
                Dummy m_dummy;
                T m_value;
            };

            bool m_hasValue;
        };

        /// @brief The storage base manages the actual storage, and correctly propagates
        ///        trivial destruction from T.
        ///        This case is for when T is trivially destructible.
        template<typename T>
        struct OptionalStorageBase<T, true>
        {
            constexpr OptionalStorageBase() noexcept
                : m_dummy()
            {}

            template<typename... U>
            constexpr OptionalStorageBase([[maybe_unused]] std::in_place_t _, U&&... u)
                : m_value(std::forward<U>(u)...), m_hasValue(true)
            {}

            //No destructor, so this class is trivially destructible

            struct Dummy {};
            union
            {
                Dummy m_dummy;
                T m_value;
            };

            bool m_hasValue = false;
        };

        //This base class provides some handy member functions which can be used in
        //further derived classes
        template<typename T>
        struct OptionalOperationsBase : OptionalStorageBase<T>
        {
            using OptionalStorageBase<T>::OptionalStorageBase;

            constexpr void HardReset() noexcept
            {
                std::destroy_at(std::addressof(Get()));
                this->m_hasValue = false;
            }

            template<typename... Args>
            constexpr void Construct(Args&&... args)
            {
                std::construct_at(std::addressof(this->m_value), std::forward<Args>(args)...);
                this->m_hasValue = true;
            }

            template<typename Opt>
            constexpr void Assign(Opt&& rhs)
            {
                if (this->HasValue())
                {
                    if (rhs.HasValue())
                        this->m_value = std::forward<Opt>(rhs).Get();
                    else
                    {
                        std::destroy_at(std::addressof(this->m_value));
                        this->m_hasValue = false;
                    }
                }
                else if (rhs.HasValue())
                    Construct(std::forward<Opt>(rhs).Get());
            }

            [[nodiscard]] constexpr bool HasValue() const
            {
                return this->m_hasValue;
            }

            [[nodiscard]] constexpr T& Get() &
            {
                return this->m_value;
            }
            [[nodiscard]] constexpr const T& Get() const &
            {
                return this->m_value;
            }
            [[nodiscard]] constexpr T&& Get() &&
            {
                return std::move(this->m_value);
            }
            [[nodiscard]] constexpr const T&& Get() const &&
            {
                return std::move(this->m_value);
            }
        };

        /// @brief This class manages conditionally having a trivial copy constructor.
        ///        This specialization is for when T is trivially copy constructible.
        template<typename T, bool = std::is_trivially_copy_constructible_v<T>>
        struct OptionalCopyBase : OptionalOperationsBase<T>
        {
            using OptionalOperationsBase<T>::OptionalOperationsBase;
        };

        /// @brief This class manages conditionally having a trivial copy constructor.
        ///        This specialization is for when T is not trivially copy constructible
        template<typename T>
        struct OptionalCopyBase<T, false> : OptionalOperationsBase<T>
        {
            using OptionalOperationsBase<T>::OptionalOperationsBase;

            constexpr OptionalCopyBase() = default;
            constexpr OptionalCopyBase(const OptionalCopyBase& rhs)
                : OptionalOperationsBase<T>()
            {
                if (rhs.HasValue())
                    this->Construct(rhs.Get());
                else
                    this->m_hasValue = false;
            }

            constexpr OptionalCopyBase(OptionalCopyBase&& rhs) = default;
            constexpr OptionalCopyBase& operator=(const OptionalCopyBase& rhs) = default;
            constexpr OptionalCopyBase& operator=(OptionalCopyBase&& rhs) = default;
        };

        /// @brief This class manages conditionally having a trivial move constructor.
        ///        This specialization is for when T is trivially move constructible.
        template<typename T, bool = std::is_trivially_move_constructible_v<T>>
        struct OptionalMoveBase : OptionalCopyBase<T>
        {
            using OptionalCopyBase<T>::OptionalCopyBase;
        };

        /// @brief This class manages conditionally having a trivial move constructor.
        ///        This specialization is for when T is not trivially move constructible.
        template<typename T>
        struct OptionalMoveBase<T, false> : OptionalCopyBase<T>
        {
            using OptionalCopyBase<T>::OptionalCopyBase;

            constexpr OptionalMoveBase() = default;
            constexpr OptionalMoveBase(const OptionalMoveBase& rhs) = default;

            constexpr OptionalMoveBase(OptionalMoveBase&& rhs) noexcept(std::is_nothrow_move_constructible_v<T>)
            {
                if (rhs.HasValue())
                    this->Construct(std::move(rhs.Get()));
                else
                    this->m_hasValue = false;
            }
            constexpr OptionalMoveBase& operator=(const OptionalMoveBase& rhs) = default;
            constexpr OptionalMoveBase& operator=(OptionalMoveBase&& rhs) = default;
        };

        /// @brief This class manages conditionally having a trivial copy assignment operator.
        template<typename T, bool = std::is_trivially_copy_assignable_v<T> &&
                                    std::is_trivially_copy_constructible_v<T> &&
                                    std::is_trivially_destructible_v<T>>
        struct OptionalCopyAssignBase : OptionalMoveBase<T>
        {
            using OptionalMoveBase<T>::OptionalMoveBase;
        };

        /// @brief This class manages conditionally having a trivial copy assignment operator.
        template<typename T>
        struct OptionalCopyAssignBase<T, false> : OptionalMoveBase<T>
        {
            using OptionalMoveBase<T>::OptionalMoveBase;

            constexpr OptionalCopyAssignBase() = default;
            constexpr OptionalCopyAssignBase(const OptionalCopyAssignBase& rhs) = default;

            constexpr OptionalCopyAssignBase(OptionalCopyAssignBase&& rhs) = default;
            constexpr OptionalCopyAssignBase& operator=(const OptionalCopyAssignBase& rhs)
            {
                if(this == &rhs)
                    return *this;

                this->Assign(rhs);
                return *this;
            }
            constexpr OptionalCopyAssignBase& operator=(OptionalCopyAssignBase&& rhs) = default;
        };

        /// @brief This class manages conditionally having a trivial move assignment operator.
        template<typename T, bool = std::is_trivially_destructible_v<T> &&
                                    std::is_trivially_move_constructible_v<T> &&
                                    std::is_trivially_move_assignable_v<T>>
        struct OptionalMoveAssignBase : OptionalCopyAssignBase<T>
        {
            using OptionalCopyAssignBase<T>::OptionalCopyAssignBase;
        };

        /// @brief This class manages conditionally having a trivial move assignment operator.
        template<typename T>
        struct OptionalMoveAssignBase<T, false> : OptionalCopyAssignBase<T>
        {
            using OptionalCopyAssignBase<T>::OptionalCopyAssignBase;

            constexpr OptionalMoveAssignBase() = default;
            constexpr OptionalMoveAssignBase(const OptionalMoveAssignBase& rhs) = default;

            constexpr OptionalMoveAssignBase(OptionalMoveAssignBase&& rhs) = default;

            constexpr OptionalMoveAssignBase& operator=(const OptionalMoveAssignBase& rhs) = default;

            constexpr OptionalMoveAssignBase& operator=(OptionalMoveAssignBase&& rhs) noexcept(std::is_nothrow_move_constructible_v<T> &&
                                                                                               std::is_nothrow_move_assignable_v<T>)
            {
                this->Assign(std::move(rhs));
                return *this;
            }
        };

        /// @brief OptionalDeleteCtorBase will conditionally delete copy and move
        ///        constructors depending on whether T is copy/move constructible.
        template<typename T, bool EnableCopy = std::is_copy_constructible_v<T>,
                             bool EnableMove = std::is_move_constructible_v<T>>
        struct OptionalDeleteCtorBase
        {
            constexpr OptionalDeleteCtorBase() = default;
            constexpr OptionalDeleteCtorBase(const OptionalDeleteCtorBase&) = default;
            constexpr OptionalDeleteCtorBase(OptionalDeleteCtorBase&&) noexcept = default;
            constexpr OptionalDeleteCtorBase& operator=(const OptionalDeleteCtorBase&) = default;
            constexpr OptionalDeleteCtorBase& operator=(OptionalDeleteCtorBase&&) noexcept = default;
        };

        /// @brief OptionalDeleteCtorBase will conditionally delete copy and move
        ///        constructors depending on whether T is copy/move constructible.
        template<typename T>
        struct OptionalDeleteCtorBase<T, true, false>
        {
            constexpr OptionalDeleteCtorBase() = default;
            constexpr OptionalDeleteCtorBase(const OptionalDeleteCtorBase&) = default;
            consteval OptionalDeleteCtorBase(OptionalDeleteCtorBase&&) noexcept = delete;
            constexpr OptionalDeleteCtorBase& operator=(const OptionalDeleteCtorBase&) = default;
            constexpr OptionalDeleteCtorBase& operator=(OptionalDeleteCtorBase&&) noexcept = default;
        };

        /// @brief OptionalDeleteCtorBase will conditionally delete copy and move
        ///        constructors depending on whether T is copy/move constructible.
        template<typename T>
        struct OptionalDeleteCtorBase<T, false, true>
        {
            constexpr OptionalDeleteCtorBase() = default;
            consteval OptionalDeleteCtorBase(const OptionalDeleteCtorBase&) = delete;
            constexpr OptionalDeleteCtorBase(OptionalDeleteCtorBase&&) noexcept = default;
            constexpr OptionalDeleteCtorBase& operator=(const OptionalDeleteCtorBase&) = default;
            constexpr OptionalDeleteCtorBase& operator=(OptionalDeleteCtorBase&&) noexcept = default;
        };

        /// @brief OptionalDeleteCtorBase will conditionally delete copy and move
        ///        constructors depending on whether T is copy/move constructible.
        template<typename T>
        struct OptionalDeleteCtorBase<T, false, false>
        {
            constexpr OptionalDeleteCtorBase() = default;
            consteval OptionalDeleteCtorBase(const OptionalDeleteCtorBase&) = delete;
            consteval OptionalDeleteCtorBase(OptionalDeleteCtorBase&&) noexcept = delete;
            constexpr OptionalDeleteCtorBase& operator=(const OptionalDeleteCtorBase&) = default;
            constexpr OptionalDeleteCtorBase& operator=(OptionalDeleteCtorBase&&) noexcept = default;
        };

        /// @brief OptionalDeleteAssignBase will conditionally delete copy and move constructors
        ///        depending on whether T is copy/move constructible + assignable.
        template <class T, bool EnableCopy = (std::is_copy_constructible_v<T> && std::is_copy_assignable_v<T>),
                           bool EnableMove = (std::is_move_constructible_v<T> && std::is_move_assignable_v<T>)>
        struct OptionalDeleteAssignBase
        {
            constexpr OptionalDeleteAssignBase() = default;
            constexpr OptionalDeleteAssignBase(const OptionalDeleteAssignBase&) = default;
            constexpr OptionalDeleteAssignBase(OptionalDeleteAssignBase&&) noexcept = default;
            constexpr OptionalDeleteAssignBase& operator=(const OptionalDeleteAssignBase&) = default;
            constexpr OptionalDeleteAssignBase& operator=(OptionalDeleteAssignBase&&) noexcept = default;
        };

        /// @brief OptionalDeleteAssignBase will conditionally delete copy and move constructors
        ///        depending on whether T is copy/move constructible + assignable.
        template<typename T>
        struct OptionalDeleteAssignBase<T, true, false>
        {
            constexpr OptionalDeleteAssignBase() = default;
            constexpr OptionalDeleteAssignBase(const OptionalDeleteAssignBase&) = default;
            constexpr OptionalDeleteAssignBase(OptionalDeleteAssignBase&&) noexcept = default;
            constexpr OptionalDeleteAssignBase& operator=(const OptionalDeleteAssignBase&) = default;
            consteval OptionalDeleteAssignBase& operator=(OptionalDeleteAssignBase&&) noexcept = delete;
        };

        /// @brief OptionalDeleteAssignBase will conditionally delete copy and move constructors
        ///        depending on whether T is copy/move constructible + assignable.
        template<typename T>
        struct OptionalDeleteAssignBase<T, false, true>
        {
            constexpr OptionalDeleteAssignBase() = default;
            constexpr OptionalDeleteAssignBase(const OptionalDeleteAssignBase&) = default;
            constexpr OptionalDeleteAssignBase(OptionalDeleteAssignBase&&) noexcept = default;
            consteval OptionalDeleteAssignBase& operator=(const OptionalDeleteAssignBase&) = delete;
            constexpr OptionalDeleteAssignBase& operator=(OptionalDeleteAssignBase&&) noexcept = default;
        };

        /// @brief OptionalDeleteAssignBase will conditionally delete copy and move constructors
        ///        depending on whether T is copy/move constructible + assignable.
        template<typename T>
        struct OptionalDeleteAssignBase<T, false, false>
        {
            constexpr OptionalDeleteAssignBase() = default;
            constexpr OptionalDeleteAssignBase(const OptionalDeleteAssignBase&) = default;
            constexpr OptionalDeleteAssignBase(OptionalDeleteAssignBase&&) noexcept = default;
            consteval OptionalDeleteAssignBase& operator=(const OptionalDeleteAssignBase&) = delete;
            consteval OptionalDeleteAssignBase& operator=(OptionalDeleteAssignBase&&) noexcept = delete;
        };
    }

    /// @brief TRAP::nullopt_t is an empty class type used to indicate optional type with
    ///        uninitialized state. In particular TRAP::Optional has a constructor with nullopt_t
    ///        as a single argument, which creates an optional that does not contain a value.
    ///
    ///        TRAP::nullopt_t must be a non-aggregate LiteralType and cannot have a default
    ///        constructor or an initializer-list constructor.
    ///
    ///        It must have a constexpr constructor that takes some implementation-defined literal type.
    /// @note The constraints on nullopt_t's constructors exist to support both op = {}; and
    ///       op = NullOpt; as the syntax for disengaging an optional object.
    struct nullopt_t
    {
        struct DoNotUse {};
        consteval explicit nullopt_t([[maybe_unused]] DoNotUse _1, [[maybe_unused]] DoNotUse _2) noexcept
        {}
    };

    /// @brief TRAP::NullOpt is a constant of type TRAP::nullopt_t that is used to indicate optional
    ///        type with uninitialized state.
    inline constexpr nullopt_t NullOpt{nullopt_t::DoNotUse{}, nullopt_t::DoNotUse{}};

    /// @brief Defines a type of object to be thrown by TRAP::Optional::Value() when accessing
    ///        an optional object that does not contain a value.
    class BadOptionalAccess final : public std::exception
    {
    public:
        /// @brief Constructs a new BadOptionalAccess object with an implementation-defined
        ///        null-terminated byte string which is accessible through what().
        ///
        ///        Default constructor.
        BadOptionalAccess() noexcept = default;
        /// @brief Destroys the exception object.
        ~BadOptionalAccess() override = default;

        BadOptionalAccess(const BadOptionalAccess& other) noexcept = default;
        BadOptionalAccess& operator=(const BadOptionalAccess& other) noexcept = default;

        BadOptionalAccess(BadOptionalAccess&& other) noexcept = delete;
        BadOptionalAccess& operator=(BadOptionalAccess&& other) noexcept = delete;

        /// @brief Returns the explanatory string.
        /// @return Pointer to a null-terminated string with explanatory information.
        ///         The string is suitable for conversion and display as a std::wstring.
        ///         The pointer is guaranteed to be valid at least until the exception
        ///         object from which it is obtained is destroyed, or until a non-const
        ///         member function (e.g. copy assignment operator) on the exception object
        ///         is called.
        [[nodiscard]] constexpr const char *what() const noexcept override
        {
            return "Optional has no value";
        }
    };

    //An optional object is an object that contains the storage for another
    //object and manages the lifetime of this contained object, if any. The
    //contained object may be initialized after the optional object has been
    //initialized, and may be destroyed before the optional object has been
    //destroyed. The initialization state of the contained object is tracked by
    //the optional object.

    /// @brief The class template TRAP::Optional manages an optional contained value,
    ///        i.e. a value that may or may not be present.
    ///
    ///        A common use case for optional is the return value of a function that may fail.
    ///        As opposed to other approaches, such as std::pair<T, bool>, optional handles
    ///        expensive-to-construct objects well and is more readable, as the intent is
    ///        expressed explicitly.
    ///
    ///        Any instance of Optional<T> at any given point in time either contains a value
    ///        or does not contain a value.
    ///
    ///        If an Optional<T> contains a value, the value is guaranteed to be allocated as part
    ///        of the optional object footprint, i.e. no dynamic memory allocation ever takes place.
    ///        Thus, an optional object models an object, not a pointer, even though
    ///        operator*() and operator->() are defined.
    ///
    ///        When an object of type Optional<T> is contextually converted to bool, the conversion
    ///        returns true if the object contains a value and false if it does not contain a value.
    ///
    ///        The optional object contains a value in the following conditions:
    ///            - The object is initialized with/assigned from a value of type T or another
    ///              optional that contains a value.
    ///
    ///        The object does not contain a value in the following conditions:
    ///            - The object is default-initialized
    ///            - The object is initialized with/assigned from a value of type TRAP::nullopt_t or
    ///              an optional object that does not contain a value.
    ///            - The member function Reset() is called.
    ///
    ///        A program is ill-formed if it instantiates an optional with the (possibly cv-qualified)
    ///        tag types TRAP::nullopt_t or std::in_place_t.
    /// @tparam T The type of the value to manage initialization state for.
    template<typename T>
    class Optional : private INTERNAL::OptionalMoveAssignBase<T>,
                     private INTERNAL::OptionalDeleteCtorBase<T>,
                     private INTERNAL::OptionalDeleteAssignBase<T>
    {
        using Base = INTERNAL::OptionalMoveAssignBase<T>;

        static_assert(!std::is_same_v<T, std::in_place_t>, "Instantiation of Optional with in_place_t is ill-formed!");
        static_assert(!std::is_same_v<std::decay_t<T>, nullopt_t>, "Instantiation of Optional with nullopt_t is ill-formed!");

    public:
        /// @brief If *this contains a value, invokes f with the contained value as an argument,
        ///        and returns the result of that invocation; otherwise, returns an empty TRAP::Optional.
        ///
        ///        The return type (see below) must be a specialization of TRAP::Optional (unlike Transform()).
        ///        Otherwise, the program is ill-formed.
        /// @param f A suitable function or Callable object that returns an TRAP::Optional.
        /// @return The result of f or an empty TRAP::Optional, as described above.
        /// @note Some languages call this operation flatmap.
        template<typename F>
        requires std::invocable<F, T&>
        [[nodiscard]] constexpr auto AndThen(F&& f) &
        {
            using result = std::invoke_result_t<F, T&>;
            static_assert(INTERNAL::IsOptional<result>::value, "F must return an Optional!");

            return HasValue() ? std::invoke(std::forward<F>(f), **this) : result(NullOpt);
        }

        /// @brief If *this contains a value, invokes f with the contained value as an argument,
        ///        and returns the result of that invocation; otherwise, returns an empty TRAP::Optional.
        ///
        ///        The return type (see below) must be a specialization of TRAP::Optional (unlike Transform()).
        ///        Otherwise, the program is ill-formed.
        /// @param f A suitable function or Callable object that returns an TRAP::Optional.
        /// @return The result of f or an empty TRAP::Optional, as described above.
        /// @note Some languages call this operation flatmap.
        template<typename F>
        requires std::invocable<F, T&&>
        [[nodiscard]] constexpr auto AndThen(F&& f) &&
        {
            using result = std::invoke_result_t<F, T&&>;
            static_assert(INTERNAL::IsOptional<result>::value, "F must return an Optional!");

            return HasValue() ? std::invoke(std::forward<F>(f), std::move(**this)) : result(NullOpt);
        }

        /// @brief If *this contains a value, invokes f with the contained value as an argument,
        ///        and returns the result of that invocation; otherwise, returns an empty TRAP::Optional.
        ///
        ///        The return type (see below) must be a specialization of TRAP::Optional (unlike Transform()).
        ///        Otherwise, the program is ill-formed.
        /// @param f A suitable function or Callable object that returns an TRAP::Optional.
        /// @return The result of f or an empty TRAP::Optional, as described above.
        /// @note Some languages call this operation flatmap.
        template<typename F>
        requires std::invocable<F, const T&>
        [[nodiscard]] constexpr auto AndThen(F&& f) const &
        {
            using result = std::invoke_result_t<F, const T&>;
            static_assert(INTERNAL::IsOptional<result>::value, "F must return an Optional!");

            return HasValue() ? std::invoke(std::forward<F>(f), **this) : result(NullOpt);
        }

        /// @brief If *this contains a value, invokes f with the contained value as an argument,
        ///        and returns the result of that invocation; otherwise, returns an empty TRAP::Optional.
        ///
        ///        The return type (see below) must be a specialization of TRAP::Optional (unlike Transform()).
        ///        Otherwise, the program is ill-formed.
        /// @param f A suitable function or Callable object that returns an TRAP::Optional.
        /// @return The result of f or an empty TRAP::Optional, as described above.
        /// @note Some languages call this operation flatmap.
        template<typename F>
        requires std::invocable<F, const T&&>
        [[nodiscard]] constexpr auto AndThen(F&& f) const &&
        {
            using result = std::invoke_result_t<F, const T&&>;
            static_assert(INTERNAL::IsOptional<result>::value, "F must return an Optional!");

            return HasValue() ? std::invoke(std::forward<F>(f), std::move(**this)) : result(NullOpt);
        }

        /// @brief If *this contains a value, invokes f with the contained value as an argument,
        ///        and returns an TRAP::Optional that contains the result of that invocation;
        ///        Otherwise, returns an empty TRAP::Optional.
        ///
        ///        The type of contained value in the result must be anon-array object type,
        ///        and must not be std::in_place_t or TRAP::nullopt_t.
        ///        Otherwise, the program is ill-formed.
        /// @param f A suitable function or Callable object.
        /// @return An TRAP::Optional containing the result of f or an empty TRAP::Optional,
        ///         as described above.
        /// @note Some languages call this operation map.
        template<typename F>
        requires std::invocable<F, T&>
        [[nodiscard]] constexpr auto Transform(F&& f) &
        {
            return OptionalTransformImpl(*this, std::forward<F>(f));
        }

        /// @brief If *this contains a value, invokes f with the contained value as an argument,
        ///        and returns an TRAP::Optional that contains the result of that invocation;
        ///        Otherwise, returns an empty TRAP::Optional.
        ///
        ///        The type of contained value in the result must be anon-array object type,
        ///        and must not be std::in_place_t or TRAP::nullopt_t.
        ///        Otherwise, the program is ill-formed.
        /// @param f A suitable function or Callable object.
        /// @return An TRAP::Optional containing the result of f or an empty TRAP::Optional,
        ///         as described above.
        /// @note Some languages call this operation map.
        template<typename F>
        requires std::invocable<F, T&&>
        [[nodiscard]] constexpr auto Transform(F&& f) &&
        {
            return OptionalTransformImpl(std::move(*this), std::forward<F>(f));
        }

        /// @brief If *this contains a value, invokes f with the contained value as an argument,
        ///        and returns an TRAP::Optional that contains the result of that invocation;
        ///        Otherwise, returns an empty TRAP::Optional.
        ///
        ///        The type of contained value in the result must be anon-array object type,
        ///        and must not be std::in_place_t or TRAP::nullopt_t.
        ///        Otherwise, the program is ill-formed.
        /// @param f A suitable function or Callable object.
        /// @return An TRAP::Optional containing the result of f or an empty TRAP::Optional,
        ///         as described above.
        /// @note Some languages call this operation map.
        template<typename F>
        requires std::invocable<F, const T&>
        [[nodiscard]] constexpr auto Transform(F&& f) const &
        {
            return OptionalTransformImpl(*this, std::forward<F>(f));
        }

        /// @brief If *this contains a value, invokes f with the contained value as an argument,
        ///        and returns an TRAP::Optional that contains the result of that invocation;
        ///        Otherwise, returns an empty TRAP::Optional.
        ///
        ///        The type of contained value in the result must be anon-array object type,
        ///        and must not be std::in_place_t or TRAP::nullopt_t.
        ///        Otherwise, the program is ill-formed.
        /// @param f A suitable function or Callable object.
        /// @return An TRAP::Optional containing the result of f or an empty TRAP::Optional,
        ///         as described above.
        /// @note Some languages call this operation map.
        template<typename F>
        requires std::invocable<F, const T&&>
        [[nodiscard]] constexpr auto Transform(F&& f) const &&
        {
            return OptionalTransformImpl(std::move(*this), std::forward<F>(f));
        }

        /// @brief Returns *this if it contains a value. Otherwise, returns the result of f.
        /// @param f A function or Callable object that returns an TRAP::Optional<T>.
        /// @return *this or the result of f, as described above.
        template<typename F, INTERNAL::EnableIfRetVoid<F>* = nullptr>
        requires std::invocable<F>
        [[nodiscard]] constexpr Optional<T> OrElse(F&& f) &
        {
            if (HasValue())
                return *this;

            std::forward<F>(f)();
            return NullOpt;
        }

        /// @brief Returns *this if it contains a value. Otherwise, returns the result of f.
        /// @param f A function or Callable object that returns an TRAP::Optional<T>.
        /// @return *this or the result of f, as described above.
        template<typename F, INTERNAL::DisableIfRetVoid<F>* = nullptr>
        requires std::invocable<F>
        [[nodiscard]] constexpr Optional<T> OrElse(F&& f) &
        {
            return HasValue() ? *this : std::forward<F>(f)();
        }

        /// @brief Returns *this if it contains a value. Otherwise, returns the result of f.
        /// @param f A function or Callable object that returns an TRAP::Optional<T>.
        /// @return *this or the result of f, as described above.
        template<typename F, INTERNAL::EnableIfRetVoid<F>* = nullptr>
        requires std::invocable<F>
        [[nodiscard]] constexpr Optional<T> OrElse(F&& f) &&
        {
            if (HasValue())
                return std::move(*this);

            std::forward<F>(f)();
            return NullOpt;
        }

        /// @brief Returns *this if it contains a value. Otherwise, returns the result of f.
        /// @param f A function or Callable object that returns an TRAP::Optional<T>.
        /// @return *this or the result of f, as described above.
        template<typename F, INTERNAL::DisableIfRetVoid<F>* = nullptr>
        requires std::invocable<F>
        [[nodiscard]] constexpr Optional<T> OrElse(F&& f) &&
        {
            return HasValue() ? std::move(*this) : std::forward<F>(f)();
        }

        /// @brief Returns *this if it contains a value. Otherwise, returns the result of f.
        /// @param f A function or Callable object that returns an TRAP::Optional<T>.
        /// @return *this or the result of f, as described above.
        template<typename F, INTERNAL::EnableIfRetVoid<F>* = nullptr>
        requires std::invocable<F>
        [[nodiscard]] constexpr Optional<T> OrElse(F&& f) const &
        {
            if (HasValue())
                return *this;

            std::forward<F>(f)();
            return NullOpt;
        }

        /// @brief Returns *this if it contains a value. Otherwise, returns the result of f.
        /// @param f A function or Callable object that returns an TRAP::Optional<T>.
        /// @return *this or the result of f, as described above.
        template<typename F, INTERNAL::DisableIfRetVoid<F>* = nullptr>
        requires std::invocable<F>
        [[nodiscard]] constexpr Optional<T> OrElse(F&& f) const &
        {
            return HasValue() ? *this : std::forward<F>(f)();
        }

        /// @brief Returns *this if it contains a value. Otherwise, returns the result of f.
        /// @param f A function or Callable object that returns an TRAP::Optional<T>.
        /// @return *this or the result of f, as described above.
        template<typename F, INTERNAL::EnableIfRetVoid<F>* = nullptr>
        requires std::invocable<F>
        [[nodiscard]] constexpr Optional<T> OrElse(F&& f) const &&
        {
            if (HasValue())
                return std::move(*this);

            std::forward<F>(f)();
            return NullOpt;
        }

        /// @brief Returns *this if it contains a value. Otherwise, returns the result of f.
        /// @param f A function or Callable object that returns an TRAP::Optional<T>.
        /// @return *this or the result of f, as described above.
        template<typename F, INTERNAL::DisableIfRetVoid<F>* = nullptr>
        requires std::invocable<F>
        [[nodiscard]] constexpr Optional<T> OrElse(F&& f) const &&
        {
            return HasValue() ? std::move(*this) : std::forward<F>(f)();
        }

        /// @brief Returns the result of f if optional contains a value. Otherwise returns u.
        /// @param f A function or Callable object that returns a value of type U.
        /// @param u Value to return if no value is contained in the optional.
        /// @return Result of f or value u as described above.
        template<typename F, typename U>
        requires std::invocable<F, T&>
        [[nodiscard]] constexpr U TransformOr(F&& f, U&& u) &
        {
            return HasValue() ? std::invoke(std::forward<F>(f), **this) : std::forward<U>(u);
        }

        /// @brief Returns the result of f if optional contains a value. Otherwise returns u.
        /// @param f A function or Callable object that returns a value of type U.
        /// @param u Value to return if no value is contained in the optional.
        /// @return Result of f or value u as described above.
        template<typename F, typename U>
        requires std::invocable<F, T&&>
        [[nodiscard]] constexpr U TransformOr(F&& f, U&& u) &&
        {
            return HasValue() ? std::invoke(std::forward<F>(f), std::move(**this)) : std::forward<U>(u);
        }

        /// @brief Returns the result of f if optional contains a value. Otherwise returns u.
        /// @param f A function or Callable object that returns a value of type U.
        /// @param u Value to return if no value is contained in the optional.
        /// @return Result of f or value u as described above.
        template<typename F, typename U>
        requires std::invocable<F, const T&>
        [[nodiscard]] constexpr U TransformOr(F&& f, U&& u) const &
        {
            return HasValue() ? std::invoke(std::forward<F>(f), **this) : std::forward<U>(u);
        }

        /// @brief Returns the result of f if optional contains a value. Otherwise returns u.
        /// @param f A function or Callable object that returns a value of type U.
        /// @param u Value to return if no value is contained in the optional.
        /// @return Result of f or value u as described above.
        template<typename F, typename U>
        requires std::invocable<F, const T&&>
        [[nodiscard]] constexpr U TransformOr(F&& f, U&& u) const &&
        {
            return HasValue() ? std::invoke(std::forward<F>(f), std::move(**this)) : std::forward<U>(u);
        }

        /// @brief Returns the result of f if optional contains a value.
        ///        Otherwise returns the result of u.
        /// @param f A function or Callable object that returns a value of type std::invoke_result_t<U>.
        /// @param u A function or Callable object that returns a value.
        /// @return Result of f or result of u as described above.
        template<typename F, typename U>
        requires std::invocable<F, T&>
        [[nodiscard]] constexpr std::invoke_result_t<U> TransformOrElse(F&& f, U&& u) &
        {
            return HasValue() ? std::invoke(std::forward<F>(f), **this) : std::forward<U>(u)();
        }

        /// @brief Returns the result of f if optional contains a value.
        ///        Otherwise returns the result of u.
        /// @param f A function or Callable object that returns a value of type std::invoke_result_t<U>.
        /// @param u A function or Callable object that returns a value.
        /// @return Result of f or result of u as described above.
        template<typename F, typename U>
        requires std::invocable<F, T&&>
        [[nodiscard]] constexpr std::invoke_result_t<U> TransformOrElse(F&& f, U&& u) &&
        {
            return HasValue() ? std::invoke(std::forward<F>(f), std::move(**this)) : std::forward<U>(u)();
        }

        /// @brief Returns the result of f if optional contains a value.
        ///        Otherwise returns the result of u.
        /// @param f A function or Callable object that returns a value of type std::invoke_result_t<U>.
        /// @param u A function or Callable object that returns a value.
        /// @return Result of f or result of u as described above.
        template<typename F, typename U>
        requires std::invocable<F, const T&>
        [[nodiscard]] constexpr std::invoke_result_t<U> TransformOrElse(F&& f, U&& u) const &
        {
            return HasValue() ? std::invoke(std::forward<F>(f), **this) : std::forward<U>(u)();
        }

        /// @brief Returns the result of f if optional contains a value.
        ///        Otherwise returns the result of u.
        /// @param f A function or Callable object that returns a value of type std::invoke_result_t<U>.
        /// @param u A function or Callable object that returns a value.
        /// @return Result of f or result of u as described above.
        template<typename F, typename U>
        requires std::invocable<F, const T&&>
        [[nodiscard]] constexpr std::invoke_result_t<U> TransformOrElse(F&& f, U&& u) const &&
        {
            return HasValue() ? std::invoke(std::forward<F>(f), std::move(**this)) : std::forward<U>(u)();
        }

        /// @brief Returns the value u if the optional contains a value.
        ///        Otherwise return an empty optional.
        /// @param u Value to return if optional doesnt contain a value.
        /// @return Value u or TRAP::NullOpt as described above.
        template<typename U>
        [[nodiscard]] constexpr Optional<std::decay_t<U>> Conjunction(U&& u) const
        {
            using result = Optional<std::decay_t<U>>;
            return HasValue() ? result{std::forward<U>(u)} : result{NullOpt};
        }

        /// @brief Returns rhs if the optional doesnt contain a value.
        ///        Otherwise returns the currently contained value.
        /// @param rhs Optional to return if this optional doesnt contain a value.
        /// @return Optional rhs or contained value as described above.
        [[nodiscard]] constexpr Optional Disjunction(const Optional& rhs) &
        {
            return HasValue() ? *this : rhs;
        }

        /// @brief Returns rhs if the optional doesnt contain a value.
        ///        Otherwise returns the currently contained value.
        /// @param rhs Optional to return if this optional doesnt contain a value.
        /// @return Optional rhs or contained value as described above.
        [[nodiscard]] constexpr Optional Disjunction(const Optional& rhs) const &
        {
            return HasValue() ? *this : rhs;
        }

        /// @brief Returns rhs if the optional doesnt contain a value.
        ///        Otherwise returns the currently contained value.
        /// @param rhs Optional to return if this optional doesnt contain a value.
        /// @return Optional rhs or contained value as described above.
        [[nodiscard]] constexpr Optional Disjunction(const Optional& rhs) &&
        {
            return HasValue() ? std::move(*this) : rhs;
        }

        /// @brief Returns rhs if the optional doesnt contain a value.
        ///        Otherwise returns the currently contained value.
        /// @param rhs Optional to return if this optional doesnt contain a value.
        /// @return Optional rhs or contained value as described above.
        [[nodiscard]] constexpr Optional Disjunction(const Optional& rhs) const &&
        {
            return HasValue() ? std::move(*this) : rhs;
        }

        /// @brief Returns rhs if the optional doesnt contain a value.
        ///        Otherwise returns the currently contained value.
        /// @param rhs Optional to return if this optional doesnt contain a value.
        /// @return Optional rhs or contained value as described above.
        [[nodiscard]] constexpr Optional Disjunction(Optional&& rhs) &
        {
            return HasValue() ? *this : std::move(rhs);
        }

        /// @brief Returns rhs if the optional doesnt contain a value.
        ///        Otherwise returns the currently contained value.
        /// @param rhs Optional to return if this optional doesnt contain a value.
        /// @return Optional rhs or contained value as described above.
        [[nodiscard]] constexpr Optional Disjunction(Optional&& rhs) const &
        {
            return HasValue() ? *this : std::move(rhs);
        }

        /// @brief Returns rhs if the optional doesnt contain a value.
        ///        Otherwise returns the currently contained value.
        /// @param rhs Optional to return if this optional doesnt contain a value.
        /// @return Optional rhs or contained value as described above.
        [[nodiscard]] constexpr Optional Disjunction(Optional&& rhs) &&
        {
            return HasValue() ? std::move(*this) : std::move(rhs);
        }

        /// @brief Returns rhs if the optional doesnt contain a value.
        ///        Otherwise returns the currently contained value.
        /// @param rhs Optional to return if this optional doesnt contain a value.
        /// @return Optional rhs or contained value as described above.
        [[nodiscard]] constexpr Optional Disjunction(Optional&& rhs) const &&
        {
            return HasValue() ? std::move(*this) : std::move(rhs);
        }

        /// @brief Take the value out of the optional, leaving it empty.
        /// @return Value of the optional.
        [[nodiscard]] constexpr Optional Take()
        {
            Optional ret = std::move(*this);
            Reset();
            return ret;
        }

        using value_type = T;

        /// @brief Constructs a new optional object.
        ///        Constructs an object that does not contain a value.
        constexpr Optional() noexcept = default;

        /// @brief Constructs a new optional object.
        ///        Constructs an object that does not contain a value.
        constexpr Optional([[maybe_unused]] nullopt_t _) noexcept
        {}

        /// @brief Constructs a new optional object.
        ///        Copy constructor: If other contains a value, initializes the contained
        ///        value as if direct-initializing (but not direct-list-initializing) an
        ///        object of type T with the expression *other. If other does not contain
        ///        a value, constructs an object that does not contain a value.
        constexpr Optional(const Optional& rhs) = default;

        /// @brief Constructs a new optional object.
        ///        Move constructor: If other contains a value, initializes the contained
        ///        value as if direct-initializing (but not direct-list-initializing) an
        ///        object of type T with the expression std::move(*other) and does not make
        ///        other empty: a moved-from TRAP::Optional still contains a value, but the
        ///        value itself is moved from. If other does not contain a value, constructs
        ///        an object that does not contain a value.
        constexpr Optional(Optional&& rhs) = default;

        /// @brief Constructs a new optional object.
        ///        Constructs an optional object that contains a value, initialized as if
        ///        direct-initializing (but not direct-list-initializing) an object of type
        ///        T from the arguments std::forward<Args>(args)....
        template<typename... Args>
        constexpr explicit Optional([[maybe_unused]] std::enable_if_t<std::is_constructible_v<T, Args...>, std::in_place_t> _,
                                    Args&&... args)
            : Base(std::in_place, std::forward<Args>(args)...)
        {}

        /// @brief Constructs a new optional object.
        ///        Constructs an optional object that contains a value, initialized as if
        ///        direct-initializing (but not direct-list-initializing) an object of type
        ///        T from the arguments il, std::forward<Args>(args)....
        template<typename U, typename... Args>
        constexpr explicit Optional([[maybe_unused]] std::enable_if_t<std::is_constructible_v<T, std::initializer_list<U> &, Args &&...>, std::in_place_t> _,
                                    std::initializer_list<U> il, Args&&... args)
        {
            this->Construct(il, std::forward<Args>(args)...);
        }

        /// @brief Constructs a new optional object.
        ///        Constructs an optional object that contains a value, initialized as if
        ///        direct-initializing (but not direct-list-initializing) an object of type
        ///        T with the expression std::forward<U>(u).
        template<typename U = T, std::enable_if_t<std::is_convertible_v<U&&, T>>* = nullptr,
                 INTERNAL::EnableForwardValue<T, U>* = nullptr>
        constexpr Optional(U&& u)
            : Base(std::in_place, std::forward<U>(u))
        {}

        /// @brief Constructs a new optional object.
        ///        Constructs an optional object that contains a value, initialized as if
        ///        direct-initializing (but not direct-list-initializing) an object of type
        ///        T with the expression std::forward<U>(u).
        template<typename U = T, std::enable_if_t<!std::is_convertible_v<U&&, T>>* = nullptr,
                 INTERNAL::EnableForwardValue<T, U>* = nullptr>
        constexpr explicit Optional(U&& u)
            : Base(std::in_place, std::forward<U>(u))
        {}

        /// @brief Constructs a new optional object.
        ///        Converting copy constructor: If other does not contain a value, constructs
        ///        an optional object that does not contain a value. Otherwise, constructs an
        ///        optional object that contains a value, initialized as if direct-initializing
        ///        (but not direct-list-initializing) an object of type T with the expression *other.
        template<typename U, INTERNAL::EnableFromOther<T, U, const U&>* = nullptr,
                 std::enable_if_t<std::is_convertible_v<const U&, T>>* = nullptr>
        constexpr Optional(const Optional<U>& rhs)
        {
            if (rhs.HasValue())
                this->Construct(*rhs);
        }

        /// @brief Constructs a new optional object.
        ///        Converting copy constructor: If other does not contain a value, constructs
        ///        an optional object that does not contain a value. Otherwise, constructs an
        ///        optional object that contains a value, initialized as if direct-initializing
        ///        (but not direct-list-initializing) an object of type T with the expression *other.
        template<typename U, INTERNAL::EnableFromOther<T, U, const U&>* = nullptr,
                 std::enable_if_t<!std::is_convertible_v<const U&, T>>* = nullptr>
        constexpr explicit Optional(const Optional<U>& rhs)
        {
            if (rhs.HasValue())
                this->Construct(*rhs);
        }

        /// @brief Constructs a new optional object.
        ///        Converting move constructor: If other does not contain a value, constructs
        ///        an optional object that does not contain a value. Otherwise, constructs an
        ///        optional object that contains a value, initialized as if direct-initializing
        ///        (but not direct-list-initializing) an object of type T with the
        ///        expression std::move(*other).
        template<typename U, INTERNAL::EnableFromOther<T, U, U&&>* = nullptr,
                 std::enable_if_t<std::is_convertible_v<U&&, T>>* = nullptr>
        constexpr Optional(Optional<U>&& rhs)
        {
            if (rhs.HasValue())
                this->Construct(std::move(*rhs));
        }

        /// @brief Constructs a new optional object.
        ///        Converting move constructor: If other does not contain a value, constructs
        ///        an optional object that does not contain a value. Otherwise, constructs an
        ///        optional object that contains a value, initialized as if direct-initializing
        ///        (but not direct-list-initializing) an object of type T with the
        ///        expression std::move(*other).
        template<typename U, INTERNAL::EnableFromOther<T, U, U&&>* = nullptr,
                 std::enable_if_t<!std::is_convertible_v<U&&, T>>* = nullptr>
        constexpr explicit Optional(Optional<U>&& rhs)
        {
            if (rhs.HasValue())
                this->Construct(std::move(*rhs));
        }

        /// @brief If the object contains a value and the type T is not trivially destructible
        ///        destroys the contained value by calling its destructor, as if by Value().T::~T().
        ///        Otherwise, does nothing.
        /// @note If T is trivially-destructible, then this destructor is also trivial, so
        ///       TRAP::Optional<T> is also trivally-destructible.
        constexpr ~Optional() = default;

        /// @brief Replaces contents of *this with the contents of other.
        ///        If *this contains a value before the call, the contained value is
        ///        destroyed by calling its destructor as if by Value().T::~T().
        ///        *this does not contain a value after this call.
        /// @return *this.
        constexpr Optional& operator=([[maybe_unused]] nullopt_t _) noexcept
        {
            if (HasValue())
            {
                std::destroy_at(std::addressof(this->m_value));
                this->m_hasValue = false;
            }

            return *this;
        }

        /// @brief Replaces contents of *this with the contents of other.
        ///        Assigns state of other.
        ///
        ///        - If both *this and other do not contain a value, the function has no effect.
        ///        - If *this contains a value, but other does not, then the contained value is
        ///          destroyed by calling its destructor.
        ///        - If other contains a value, then depending on whether *this contains a value,
        ///          the contained value is either direct-initialized or assigned from *other.
        /// @param rhs Another Optional object whose contained value to assign.
        /// @return *this.
        constexpr Optional& operator=(const Optional& rhs) = default;

        /// @brief Replaces contents of *this with the contents of other.
        ///        Assigns state of other.
        ///
        ///        - If both *this and other do not contain a value, the function has no effect.
        ///        - If *this contains a value, but other does not, then the contained value is
        ///          destroyed by calling its destructor.
        ///        - If other contains a value, then depending on whether *this contains a value,
        ///          the contained value is either direct-initialized or assigned from std::move(*other).
        ///          Note that a moved-from optional still contains a value.
        /// @param rhs Another Optional object whose contained value to assign.
        /// @return *this.
        constexpr Optional& operator=(Optional&& rhs) = default;

        /// @brief Replaces contents of *this with the contents of other.
        ///        Perfect-forwarded assignment: depending on whether *this contains
        ///        a value before the call, the contained value is either direct-initialized
        ///        from std::forward<U>(u) or assigned from std::forward<U>(u).
        /// @param u Value to assign to the contained value.
        /// @return *this
        template<typename U = T, INTERNAL::EnableAssignForward<T, U>* = nullptr>
        constexpr Optional& operator=(U&& u)
        {
            if (HasValue())
                this->m_value = std::forward<U>(u);
            else
                this->Construct(std::forward<U>(u));

            return *this;
        }

        /// @brief Replaces contents of *this with the contents of other.
        ///        Assigns the state of other.
        ///
        ///        - If both *this and other do not contain a value, the function has no effect.
        ///        - If *this contains a value, but other does not, then the contained value
        ///          is destroyed by calling its destructor. *this does not contain a value after the call.
        ///        - If other contains a value, then depending on whether *this contains a value, the
        ///          contained value is either direct-initialized or assigned from *other.
        /// @param rhs Another Optional object whose contained value to assign.
        /// @return *this.
        template<typename U, INTERNAL::EnableAssignFromOther<T, U, const U&>* = nullptr>
        constexpr Optional& operator=(const Optional<U>& rhs)
        {
            if (HasValue())
            {
                if (rhs.HasValue())
                    this->m_value = *rhs;
                else
                    this->HardReset();
            }
            else if (rhs.HasValue())
                this->Construct(*rhs);

            return *this;
        }

        /// @brief Replaces contents of *this with the contents of other.
        ///        Assigns the state of other.
        ///
        ///        - If both *this and other do not contain a value, the function has no effect.
        ///        - If *this contains a value, but other does not, then the contained value
        ///          is destroyed by calling its destructor. *this does not contain a value after the call.
        ///        - If other contains a value, then depending on whether *this contains a value, the
        ///          contained value is either direct-initialized or assigned from std::move(*other).
        ///          Note that a moved-from optional still contains a value.
        /// @param rhs Another Optional object whose contained value to assign.
        /// @return *this.
        template<typename U, INTERNAL::EnableAssignFromOther<T, U, U>* = nullptr>
        constexpr Optional& operator=(Optional<U>&& rhs)
        {
            if (HasValue())
            {
                if (rhs.HasValue())
                    this->m_value = std::move(*rhs);
                else
                    this->HardReset();
            }
            else if (rhs.HasValue())
                this->Construct(std::move(*rhs));

            return *this;
        }

        /// @brief Constructs the contained value in-place. If *this already contains a value
        ///        before the call, the contained value is destroyed by calling its destructor.
        ///
        ///        Initializes the contained value by direct-initializing (but not direct-list-initializing)
        ///        with std::forward<Args>(args)... as parameters.
        /// @param args The arguments to pass to the constructor.
        /// @return A reference to the new contained value.
        template<typename... Args>
        constexpr T& Emplace(Args&&... args)
        {
            static_assert(std::is_constructible<T, Args &&...>::value, "T must be constructible with Args!");

            *this = NullOpt;
            this->Construct(std::forward<Args>(args)...);
            return Value();
        }

        /// @brief Constructs the contained value in-place. If *this already contains a value
        ///        before the call, the contained value is destroyed by calling its destructor.
        ///
        ///        Initializes the contained value by calling its constructor with
        ///        ilist, std::forward<Args>(args)... as parameters.
        /// @param il The initializer list to pass to the constructor.
        /// @param args The arguments to pass to the constructor.
        /// @return A reference to the new contained value.
        template<typename U, typename... Args>
        constexpr std::enable_if_t<std::is_constructible_v<T, std::initializer_list<U>&, Args&&...>, T&>
        Emplace(std::initializer_list<U> il, Args&&... args)
        {
            *this = NullOpt;
            this->Construct(il, std::forward<Args>(args)...);
            return Value();
        }

        /// @brief Swaps the contents with those of other.
        ///
        ///        - If neither *this nor other contain a value, the function has no effect.
        ///        - If only one of *this and other contains a value (let's call this object in
        ///          and the other un), the contained value of un is direct-initialized from
        ///          std::move(*in), followed by destruction of the contained value of in as if by
        ///          in->T::~T(). After this call, in does not contain a value; un contains a value.
        ///        - If both *this and other contain values, the contained values are exchanged by calling
        ///          using std::swap; swap(**this, *other). T lvalues must satisfy Swappable.
        ///
        ///        The program is ill-formed if std::is_move_constructible_v<T> is false.
        /// @param rhs The Optional object to exchange the contents with.
        constexpr void Swap(Optional& rhs) noexcept(std::is_nothrow_move_constructible_v<T> && std::is_nothrow_swappable_v<T>)
        {
            using std::swap;

            if (HasValue())
            {
                if (rhs.HasValue())
                    swap(**this, *rhs);
                else
                {
                    std::construct_at(std::addressof(rhs.m_value), std::move(this->m_value));
                    std::destroy_at(std::addressof(this->m_value));
                }
            }
            else if (rhs.HasValue())
            {
                std::construct_at(std::addressof(this->m_value), std::move(rhs.m_value));
                std::destroy_at(std::addressof(rhs.m_value));
            }
            swap(this->m_hasValue, rhs.m_hasValue);
        }

        /// @brief Accesses the contained value.
        ///        Returns a pointer to the contained value.
        ///
        ///        The behaviour is undefined if *this does not contain a value.
        /// @return Pointer to the contained value.
        /// @note This operator does not check whether the optional contains a value!
        ///       You can do so manually by using HasValue() or simply operator bool().
        ///       Alternatively, if checked access is needed, Value() or ValueOr() may be used.
        [[nodiscard]] constexpr const T* operator->() const
        {
            return std::addressof(this->m_value);
        }

        /// @brief Accesses the contained value.
        ///        Returns a pointer to the contained value.
        ///
        ///        The behaviour is undefined if *this does not contain a value.
        /// @return Pointer to the contained value.
        /// @note This operator does not check whether the optional contains a value!
        ///       You can do so manually by using HasValue() or simply operator bool().
        ///       Alternatively, if checked access is needed, Value() or ValueOr() may be used.
        [[nodiscard]] constexpr T* operator->()
        {
            return std::addressof(this->m_value);
        }

        /// @brief Accesses the contained value.
        ///        Returns a reference to the contained value.
        ///
        ///        The behaviour is undefined if *this does not contain a value.
        /// @return Reference to the contained value.
        /// @note This operator does not check whether the optional contains a value!
        ///       You can do so manually by using HasValue() or simply operator bool().
        ///       Alternatively, if checked access is needed, Value() or ValueOr() may be used.
        [[nodiscard]] constexpr T& operator*() &
        {
            return this->m_value;
        }

        /// @brief Accesses the contained value.
        ///        Returns a reference to the contained value.
        ///
        ///        The behaviour is undefined if *this does not contain a value.
        /// @return Reference to the contained value.
        /// @note This operator does not check whether the optional contains a value!
        ///       You can do so manually by using HasValue() or simply operator bool().
        ///       Alternatively, if checked access is needed, Value() or ValueOr() may be used.
        [[nodiscard]] constexpr const T& operator*() const &
        {
            return this->m_value;
        }

        /// @brief Accesses the contained value.
        ///        Returns a reference to the contained value.
        ///
        ///        The behaviour is undefined if *this does not contain a value.
        /// @return Reference to the contained value.
        /// @note This operator does not check whether the optional contains a value!
        ///       You can do so manually by using HasValue() or simply operator bool().
        ///       Alternatively, if checked access is needed, Value() or ValueOr() may be used.
        [[nodiscard]] constexpr T&& operator*() &&
        {
            return std::move(this->m_value);
        }

        /// @brief Accesses the contained value.
        ///        Returns a reference to the contained value.
        ///
        ///        The behaviour is undefined if *this does not contain a value.
        /// @return Reference to the contained value.
        /// @note This operator does not check whether the optional contains a value!
        ///       You can do so manually by using HasValue() or simply operator bool().
        ///       Alternatively, if checked access is needed, Value() or ValueOr() may be used.
        [[nodiscard]] constexpr const T&& operator*() const &&
        {
            return std::move(this->m_value);
        }

        /// @brief Checks whether *this contains a value.
        /// @return true if *this contains a value, false if *this does not contain a value.
        [[nodiscard]] constexpr bool HasValue() const noexcept
        {
            return this->m_hasValue;
        }

        /// @brief Checks whether *this contains a value.
        /// @return true if *this contains a value, false if *this does not contain a value.
        [[nodiscard]] constexpr explicit operator bool() const noexcept
        {
            return this->m_hasValue;
        }

        /// @brief If *this contains a value, returns a reference to the contained value.
        ///        Otherwise, throws a TRAP::BadOptionalAccess exception.
        /// @return A reference to the contained value.
        /// @note The dereference operator operator*() does not check if this optional
        ///       contains a value, which may be more efficient than Value().
        [[nodiscard]] constexpr T& Value() &
        {
            if (HasValue())
                return this->m_value;
            TRAP_OPTIONAL_THROW(BadOptionalAccess());
        }
        /// @brief If *this contains a value, returns a reference to the contained value.
        ///        Otherwise, throws a TRAP::BadOptionalAccess exception.
        /// @return A reference to the contained value.
        /// @note The dereference operator operator*() does not check if this optional
        ///       contains a value, which may be more efficient than Value().
        [[nodiscard]] constexpr const T& Value() const &
        {
            if (HasValue())
                return this->m_value;
            TRAP_OPTIONAL_THROW(BadOptionalAccess());
        }
        /// @brief If *this contains a value, returns a reference to the contained value.
        ///        Otherwise, throws a TRAP::BadOptionalAccess exception.
        /// @return A reference to the contained value.
        /// @note The dereference operator operator*() does not check if this optional
        ///       contains a value, which may be more efficient than Value().
        [[nodiscard]] constexpr T&& Value() &&
        {
            if (HasValue())
                return std::move(this->m_value);
            TRAP_OPTIONAL_THROW(BadOptionalAccess());
        }
        /// @brief If *this contains a value, returns a reference to the contained value.
        ///        Otherwise, throws a TRAP::BadOptionalAccess exception.
        /// @return A reference to the contained value.
        /// @note The dereference operator operator*() does not check if this optional
        ///       contains a value, which may be more efficient than Value().
        [[nodiscard]] constexpr const T&& Value() const &&
        {
            if (HasValue())
                return std::move(this->m_value);
            TRAP_OPTIONAL_THROW(BadOptionalAccess());
        }

        /// @brief Returns the contained value if *this has a value, otherwise returns u.
        /// @param u The value to use in case *this is empty.
        /// @return The current value if *this has a value, or u otherwise.
        template<typename U>
        [[nodiscard]] constexpr T ValueOr(U&& u) const &
        {
            static_assert(std::is_copy_constructible<T>::value && std::is_convertible<U&&, T>::value,
                          "T must be copy constructible and convertible from U!");
            return HasValue() ? **this : static_cast<T>(std::forward<U>(u));
        }

        /// @brief Returns the contained value if *this has a value, otherwise returns u.
        /// @param u The value to use in case *this is empty.
        /// @return The current value if *this has a value, or u otherwise.
        template<typename U>
        [[nodiscard]] constexpr T ValueOr(U&& u) &&
        {
            static_assert(std::is_move_constructible<T>::value && std::is_convertible<U&&, T>::value,
                          "T must be move constructible and convertible from U!");
            return HasValue() ? std::move(**this) : static_cast<T>(std::forward<U>(u));
        }

        /// @brief Returns the contained value if *this has a value.
        ///        Otherwise returns the result of f.
        /// @param f The function or Callable object to use in case *this is empty.
        /// @return The current value if *this has a value, or the result of f otherwise.
        template<typename F>
        requires std::invocable<F>
        [[nodiscard]] constexpr T ValueOrElse(F&& f) const &
        {
            static_assert(std::same_as<T, std::invoke_result_t<F>>,
                          "F must return type T!");
            return HasValue() ? **this : std::invoke(std::forward<F>(f));
        }

        /// @brief Returns the contained value if *this has a value.
        ///        Otherwise returns the result of f.
        /// @param f The function or Callable object to use in case *this is empty.
        /// @return The current value if *this has a value, or the result of f otherwise.
        template<typename F>
        requires std::invocable<F>
        [[nodiscard]] constexpr T ValueOrElse(F&& f) &&
        {
            static_assert(std::same_as<T, std::invoke_result_t<F>>,
                          "F must return type T!");
            return HasValue() ? std::move(**this) : std::invoke(std::forward<F>(f));
        }

        /// @brief If *this contains a value, destroy that value as if by Value().T::~T().
        ///        Otherwise, there are no effects.
        ///
        ///        *this does not contain a value after this call.
        constexpr void Reset() noexcept
        {
            if (HasValue())
            {
                std::destroy_at(std::addressof(this->m_value));
                this->m_hasValue = false;
            }
        }
    };

    template<typename T>
    using OptionalRelOp_t = std::enable_if_t<std::is_convertible_v<T, bool>, bool>;
    template<typename T, typename U>
    using OptionalEq_t = OptionalRelOp_t<decltype(std::declval<const T&>() == std::declval<const U&>())>;
    template<typename T, typename U>
    using OptionalNe_t = OptionalRelOp_t<decltype(std::declval<const T&>() != std::declval<const U&>())>;
    template<typename T, typename U>
    using OptionalLt_t = OptionalRelOp_t<decltype(std::declval<const T&>() < std::declval<const U&>())>;
    template<typename T, typename U>
    using OptionalGt_t = OptionalRelOp_t<decltype(std::declval<const T&>() > std::declval<const U&>())>;
    template<typename T, typename U>
    using OptionalLe_t = OptionalRelOp_t<decltype(std::declval<const T&>() <= std::declval<const U&>())>;
    template<typename T, typename U>
    using OptionalGe_t = OptionalRelOp_t<decltype(std::declval<const T&>() >= std::declval<const U&>())>;

    /// Compares two optional objects
    template<typename T, typename U>
    inline constexpr OptionalEq_t<T, U> operator==(const Optional<T>& lhs, const Optional<U>& rhs)
    {
        return static_cast<bool>(lhs) == static_cast<bool>(rhs) && (!lhs || *lhs == *rhs);
    }
    template<typename T, typename U>
    inline constexpr OptionalNe_t<T, U> operator!=(const Optional<T>& lhs, const Optional<U>& rhs)
    {
        return static_cast<bool>(lhs) != static_cast<bool>(rhs) || (static_cast<bool>(lhs) && *lhs != *rhs);
    }
    template<typename T, typename U>
    inline constexpr OptionalLt_t<T, U> operator<(const Optional<T>& lhs, const Optional<U>& rhs)
    {
        return static_cast<bool>(rhs) && (!lhs || *lhs < *rhs);
    }
    template<typename T, typename U>
    inline constexpr OptionalGt_t<T, U> operator>(const Optional<T>& lhs, const Optional<U>& rhs)
    {
        return static_cast<bool>(lhs) && (!rhs || *lhs > *rhs);
    }
    template<typename T, typename U>
    inline constexpr OptionalLe_t<T, U> operator<=(const Optional<T>& lhs, const Optional<U>& rhs)
    {
        return !lhs || (static_cast<bool>(rhs) && *lhs <= *rhs);
    }
    template<typename T, typename U>
    inline constexpr OptionalGe_t<T, U> operator>=(const Optional<T>& lhs, const Optional<U>& rhs)
    {
        return !lhs || (static_cast<bool>(lhs) && *lhs >= *rhs);
    }

    template<typename T, std::three_way_comparable_with<T> U>
    inline constexpr std::compare_three_way_result_t<T, U> operator<=>(const Optional<T>& lhs, const Optional<U>& rhs)
    {
        return lhs && rhs ? *lhs <=> *rhs : bool(lhs) <=> bool(rhs);
    }

    /// Compares an optional to a `nullopt`
    template<typename T>
    inline constexpr bool operator==(const Optional<T>& lhs, [[maybe_unused]] nullopt_t _) noexcept
    {
        return !lhs;
    }

    template<typename T>
    inline constexpr std::strong_ordering operator<=>(const Optional<T>& lhs, [[maybe_unused]] nullopt_t _) noexcept
    {
        return bool(lhs) <=> false;
    }

    /// Compares the optional with a value.
    template<typename T, typename U>
    inline constexpr OptionalEq_t<T, U> operator==(const Optional<T>& lhs, const U& rhs)
    {
        return lhs && *lhs == rhs;
    }
    template<typename T, typename U>
    inline constexpr OptionalEq_t<U, T> operator==(const U& lhs, const Optional<T>& rhs)
    {
        return rhs && lhs == *rhs;
    }
    template<typename T, typename U>
    inline constexpr OptionalNe_t<T, U> operator!=(const Optional<T>& lhs, const U& rhs)
    {
        return !lhs || *lhs != rhs;
    }
    template<typename T, typename U>
    inline constexpr OptionalNe_t<U, T> operator!=(const U& lhs, const Optional<T>& rhs)
    {
        return !rhs || lhs != *rhs;
    }
    template<typename T, typename U>
    inline constexpr OptionalLt_t<T, U> operator<(const Optional<T>& lhs, const U& rhs)
    {
        return !lhs || *lhs < rhs;
    }
    template<typename T, typename U>
    inline constexpr OptionalLt_t<U, T> operator<(const U& lhs, const Optional<T>& rhs)
    {
        return rhs && lhs < *rhs;
    }
    template<typename T, typename U>
    inline constexpr OptionalLe_t<T, U> operator<=(const Optional<T>& lhs, const U& rhs)
    {
        return !lhs || *lhs <= rhs;
    }
    template<typename T, typename U>
    inline constexpr OptionalLe_t<U, T> operator<=(const U& lhs, const Optional<T>& rhs)
    {
        return rhs && lhs <= *rhs;
    }
    template<typename T, typename U>
    inline constexpr OptionalGt_t<T, U> operator>(const Optional<T>& lhs, const U& rhs)
    {
        return lhs && *lhs > rhs;
    }
    template<typename T, typename U>
    inline constexpr OptionalGt_t<U, T> operator>(const U& lhs, const Optional<T>& rhs)
    {
        return !rhs || lhs > *rhs;
    }
    template<typename T, typename U>
    inline constexpr OptionalGe_t<T, U> operator>=(const Optional<T>& lhs, const U& rhs)
    {
        return lhs && *lhs >= rhs;
    }
    template<typename T, typename U>
    inline constexpr OptionalGe_t<U, T> operator>=(const U& lhs, const Optional<T>& rhs)
    {
        return !rhs || lhs >= *rhs;
    }

    template<typename T, typename U>
    requires(!INTERNAL::IsOptional<U>::value) && std::three_way_comparable_with<T, U>
    constexpr std::compare_three_way_result_t<T, U> operator<=>(const Optional<T>& lhs, const U& v)
    {
        return bool(lhs) ? *lhs <=> v : std::strong_ordering::less;
    }

    namespace INTERNAL
    {
        struct IAmSecret{};
    }

    /// @brief Creates an optional object from v.
    /// @param v The value to construct optional object with.
    /// @return The constructed optional object.
    template<typename T = INTERNAL::IAmSecret, typename U,
             typename Ret = std::conditional_t<std::is_same_v<T, INTERNAL::IAmSecret>, std::decay_t<U>, T>>
    inline constexpr Optional<Ret> MakeOptional(U&& v)
    {
        return Optional<Ret>{std::forward<U>(v)};
    }

    /// @brief Creates an optional object constructed in-place from args....
    /// @param args Arguments to be passed to the constructor of T.
    /// @return The constructed optional object.
    /// @note T need not be movable due to guaranteed copy elision.
    template<typename T, typename... Args>
    inline constexpr Optional<T> MakeOptional(Args&&... args)
    {
        return Optional<T>{std::in_place, std::forward<Args>(args)...};
    }
    /// @brief Creates an optional object constructed in-place from il and args....
    /// @param il Arguments to be passed to the constructor of T.
    /// @param args Arguments to be passed to the constructor of T.
    /// @return The constructed optional object.
    /// @note T need not be movable due to guaranteed copy elision.
    template<typename T, typename U, typename... Args>
    inline constexpr Optional<T> MakeOptional(std::initializer_list<U> il, Args&&... args)
    {
        return Optional<T>{std::in_place, il, std::forward<Args>(args)...};
    }

    /// @brief One deduction guide is provided for TRAP::Optional to account for the
    ///        edge cases missed by the implicit deduction guides, in particular, non-copyable
    ///        arguments and array to pointer conversion.
    template<typename T>
    Optional(T)->Optional<T>;

    namespace INTERNAL
    {
        template<typename Opt, typename F,
                 typename Ret = decltype(std::invoke(std::declval<F>(), *std::declval<Opt>())),
                 std::enable_if_t<!std::is_void_v<Ret>>* = nullptr>
        constexpr auto OptionalTransformImpl(Opt&& opt, F&& f)
        {
            return opt.HasValue()
                    ? std::invoke(std::forward<F>(f), *std::forward<Opt>(opt))
                    : Optional<Ret>(NullOpt);
        }

        template<typename Opt, typename F,
                 typename Ret = decltype(std::invoke(std::declval<F>(), *std::declval<Opt>())),
                 std::enable_if_t<std::is_void_v<Ret>>* = nullptr>
        constexpr auto OptionalTransformImpl(Opt&& opt, F&& f)
        {
            if (opt.HasValue())
            {
                std::invoke(std::forward<F>(f), *std::forward<Opt>(opt));
                return TRAP::MakeOptional(std::monostate{});
            }

            return Optional<std::monostate>(NullOpt);
        }
    }

    /// Specialization for when `T` is a reference. `Optional<T&>` acts similarly
    /// to a `T*`, but provides more operations and shows intent more clearly.
    template<typename T>
    class Optional<T&>
    {
    public:
        template<typename F>
        requires std::invocable<F, T&>
        [[nodiscard]] constexpr auto AndThen(F&& f) &
        {
            using result = std::invoke_result_t<F, T&>;
            static_assert(INTERNAL::IsOptional<result>::value, "F must return an Optional!");

            return HasValue() ? std::invoke(std::forward<F>(f), **this) : result(NullOpt);
        }

        template<typename F>
        requires std::invocable<F, T&>
        [[nodiscard]] constexpr auto AndThen(F&& f) &&
        {
            using result = std::invoke_result_t<F, T&>;
            static_assert(INTERNAL::IsOptional<result>::value, "F must return an Optional!");

            return HasValue() ? std::invoke(std::forward<F>(f), **this) : result(NullOpt);
        }

        template<typename F>
        requires std::invocable<F, const T&>
        [[nodiscard]] constexpr auto AndThen(F&& f) const &
        {
            using result = std::invoke_result_t<F, const T&>;
            static_assert(INTERNAL::IsOptional<result>::value, "F must return an Optional!");

            return HasValue() ? std::invoke(std::forward<F>(f), **this) : result(NullOpt);
        }

        template<typename F>
        requires std::invocable<F, const T&>
        [[nodiscard]] constexpr auto AndThen(F&& f) const &&
        {
            using result = std::invoke_result_t<F, const T&>;
            static_assert(INTERNAL::IsOptional<result>::value, "F must return an Optional!");

            return HasValue() ? std::invoke(std::forward<F>(f), **this) : result(NullOpt);
        }

        template<typename F>
        requires std::invocable<F, T&>
        constexpr auto Transform(F&& f) &
        {
            return INTERNAL::OptionalTransformImpl(*this, std::forward<F>(f));
        }

        template<typename F>
        requires std::invocable<F, T&>
        constexpr auto Transform(F&& f) &&
        {
            return INTERNAL::OptionalTransformImpl(std::move(*this), std::forward<F>(f));
        }

        template<typename F>
        requires std::invocable<F, const T&>
        constexpr auto Transform(F&& f) const &
        {
            return INTERNAL::OptionalTransformImpl(*this, std::forward<F>(f));
        }

        template<typename F>
        requires std::invocable<F, const T&>
        constexpr auto Transform(F&& f) const &&
        {
            return INTERNAL::OptionalTransformImpl(std::move(*this), std::forward<F>(f));
        }

        template<typename F, INTERNAL::EnableIfRetVoid<F>* = nullptr>
        requires std::invocable<F>
        [[nodiscard]] constexpr Optional<T> OrElse(F&& f) &
        {
            if (HasValue())
                return *this;

            std::forward<F>(f)();
            return NullOpt;
        }

        template<typename F, INTERNAL::DisableIfRetVoid<F>* = nullptr>
        requires std::invocable<F>
        [[nodiscard]] constexpr Optional<T> OrElse(F&& f) &
        {
            return HasValue() ? *this : std::forward<F>(f)();
        }

        template<typename F, INTERNAL::EnableIfRetVoid<F>* = nullptr>
        requires std::invocable<F>
        [[nodiscard]] constexpr Optional<T> OrElse(F&& f) &&
        {
            if (HasValue())
                return std::move(*this);

            std::forward<F>(f)();
            return NullOpt;
        }

        template<typename F, INTERNAL::DisableIfRetVoid<F>* = nullptr>
        requires std::invocable<F>
        [[nodiscard]] constexpr Optional<T> OrElse(F&& f) &&
        {
            return HasValue() ? std::move(*this) : std::forward<F>(f)();
        }

        template<typename F, INTERNAL::EnableIfRetVoid<F>* = nullptr>
        requires std::invocable<F>
        [[nodiscard]] constexpr Optional<T> OrElse(F&& f) const &
        {
            if (HasValue())
                return *this;

            std::forward<F>(f)();
            return NullOpt;
        }

        template<typename F, INTERNAL::DisableIfRetVoid<F>* = nullptr>
        requires std::invocable<F>
        [[nodiscard]] constexpr Optional<T> OrElse(F&& f) const &
        {
            return HasValue() ? *this : std::forward<F>(f)();
        }

        template<typename F, INTERNAL::EnableIfRetVoid<F>* = nullptr>
        requires std::invocable<F>
        [[nodiscard]] constexpr Optional<T> OrElse(F&& f) const &&
        {
            if (HasValue())
                return std::move(*this);

            std::forward<F>(f)();
            return NullOpt;
        }

        template<typename F, INTERNAL::DisableIfRetVoid<F>* = nullptr>
        requires std::invocable<F>
        [[nodiscard]] constexpr Optional<T> OrElse(F&& f) const &&
        {
            return HasValue() ? std::move(*this) : std::forward<F>(f)();
        }

        template<typename F, typename U>
        requires std::invocable<F>
        [[nodiscard]] constexpr U TransformOr(F&& f, U&& u) &
        {
            return HasValue() ? std::invoke(std::forward<F>(f), **this) : std::forward<U>(u);
        }

        template<typename F, typename U>
        requires std::invocable<F>
        [[nodiscard]] constexpr U TransformOr(F&& f, U&& u) &&
        {
            return HasValue() ? std::invoke(std::forward<F>(f), std::move(**this))
                            : std::forward<U>(u);
        }

        template<typename F, typename U>
        requires std::invocable<F>
        [[nodiscard]] constexpr U TransformOr(F&& f, U&& u) const &
        {
            return HasValue() ? std::invoke(std::forward<F>(f), **this)
                            : std::forward<U>(u);
        }

        template<typename F, typename U>
        requires std::invocable<F>
        [[nodiscard]] constexpr U TransformOr(F&& f, U&& u) const &&
        {
            return HasValue() ? std::invoke(std::forward<F>(f), std::move(**this))
                            : std::forward<U>(u);
        }

        template<typename F, typename U>
        requires std::invocable<F, T&>
        [[nodiscard]] constexpr std::invoke_result_t<U> TransformOrElse(F&& f, U&& u) &
        {
            return HasValue() ? std::invoke(std::forward<F>(f), **this)
                            : std::forward<U>(u)();
        }

        template<typename F, typename U>
        requires std::invocable<F, T&>
        [[nodiscard]] constexpr std::invoke_result_t<U> TransformOrElse(F&& f, U&& u) &&
        {
            return HasValue() ? std::invoke(std::forward<F>(f), std::move(**this))
                            : std::forward<U>(u)();
        }

        template<typename F, typename U>
        requires std::invocable<F, const T&>
        [[nodiscard]] constexpr std::invoke_result_t<U> TransformOrElse(F&& f, U&& u) const &
        {
            return HasValue() ? std::invoke(std::forward<F>(f), **this)
                            : std::forward<U>(u)();
        }

        template<typename F, typename U>
        requires std::invocable<F, const T&>
        [[nodiscard]] constexpr std::invoke_result_t<U> TransformOrElse(F&& f, U&& u) const &&
        {
            return HasValue() ? std::invoke(std::forward<F>(f), std::move(**this))
                            : std::forward<U>(u)();
        }

        template<typename U>
        [[nodiscard]] constexpr Optional<std::decay_t<U>> Conjunction(U&& u) const
        {
            using result = Optional<std::decay_t<U>>;
            return HasValue() ? result{std::forward<U>(u)} : result{NullOpt};
        }

        [[nodiscard]] constexpr Optional Disjunction(const Optional& rhs) &
        {
            return HasValue() ? *this : rhs;
        }

        [[nodiscard]] constexpr Optional Disjunction(const Optional& rhs) const &
        {
            return HasValue() ? *this : rhs;
        }

        [[nodiscard]] constexpr Optional Disjunction(const Optional& rhs) &&
        {
            return HasValue() ? std::move(*this) : rhs;
        }

        [[nodiscard]] constexpr Optional Disjunction(const Optional& rhs) const &&
        {
            return HasValue() ? std::move(*this) : rhs;
        }

        [[nodiscard]] constexpr Optional Disjunction(Optional&& rhs) &
        {
            return HasValue() ? *this : std::move(rhs);
        }

        [[nodiscard]] constexpr Optional Disjunction(Optional&& rhs) const &
        {
            return HasValue() ? *this : std::move(rhs);
        }

        [[nodiscard]] constexpr Optional Disjunction(Optional&& rhs) &&
        {
            return HasValue() ? std::move(*this) : std::move(rhs);
        }

        [[nodiscard]] constexpr Optional Disjunction(Optional&& rhs) const &&
        {
            return HasValue() ? std::move(*this) : std::move(rhs);
        }

        [[nodiscard]] constexpr Optional Take()
        {
            Optional ret = std::move(*this);
            Reset();
            return ret;
        }

        using value_type = T&;

        constexpr Optional() noexcept
            : m_value(nullptr)
        {}

        constexpr Optional([[maybe_unused]] nullopt_t _) noexcept
            : m_value(nullptr)
        {}

        constexpr Optional(const Optional& rhs) noexcept = default;

        constexpr Optional(Optional&& rhs) = default;

        template<typename U = T, std::enable_if_t<!INTERNAL::IsOptional<std::decay_t<U>>::value>* = nullptr>
        constexpr Optional(U&& u)  noexcept
            : m_value(std::addressof(std::forward<U>(u)))
        {
            static_assert(std::is_lvalue_reference<U>::value, "U must be an lvalue!");
        }

        template<typename U>
        constexpr explicit Optional(const Optional<U>& rhs) noexcept
            : Optional(*rhs)
        {}

        constexpr ~Optional() = default;

        constexpr Optional& operator=([[maybe_unused]] nullopt_t _) noexcept
        {
            m_value = nullptr;
            return *this;
        }

        constexpr Optional& operator=(const Optional& rhs) = default;

        constexpr Optional& operator=(Optional&&) = default;

        template<typename U = T, std::enable_if_t<!INTERNAL::IsOptional<std::decay_t<U>>::value>* = nullptr>
        constexpr Optional& operator=(U&& u)
        {
            static_assert(std::is_lvalue_reference<U>::value, "U must be an lvalue!");
            m_value = std::addressof(std::forward<U>(u));
            return *this;
        }

        template<typename U>
        constexpr Optional& operator=(const Optional<U>& rhs) noexcept
        {
            m_value = std::addressof(rhs.Value());
            return *this;
        }

        template<typename U = T, std::enable_if_t<!INTERNAL::IsOptional<std::decay_t<U>>::value>* = nullptr>
        constexpr Optional& Emplace(U&& u) noexcept
        {
            return *this = std::forward<U>(u);
        }

        constexpr void Swap(Optional& rhs) noexcept
        {
            std::swap(m_value, rhs.m_value);
        }

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

        [[nodiscard]] constexpr const T& operator*() const noexcept
        {
            return *m_value;
        }

        [[nodiscard]] constexpr bool HasValue() const noexcept
        {
            return m_value != nullptr;
        }

        [[nodiscard]] constexpr explicit operator bool() const noexcept
        {
            return m_value != nullptr;
        }

        [[nodiscard]] constexpr T& Value()
        {
            if (HasValue())
                return *m_value;
            TRAP_OPTIONAL_THROW(BadOptionalAccess());
        }
        [[nodiscard]] constexpr const T& Value() const
        {
            if (HasValue())
                return *m_value;
            TRAP_OPTIONAL_THROW(BadOptionalAccess());
        }

        template<typename U>
        [[nodiscard]] constexpr T ValueOr(U&& u) const & noexcept
        {
            static_assert(std::is_copy_constructible<T>::value &&
                          std::is_convertible<U&&, T>::value,
                          "T must be copy constructible and convertible from U!");
            return HasValue() ? **this : static_cast<T>(std::forward<U>(u));
        }

        template<typename U>
        [[nodiscard]] constexpr T ValueOr(U&& u) && noexcept
        {
            static_assert(std::is_move_constructible<T>::value &&
                          std::is_convertible<U&&, T>::value,
                          "T must be move constructible and convertible from U!");
            return HasValue() ? **this : static_cast<T>(std::forward<U>(u));
        }

        template<typename F>
        requires std::invocable<F>
        [[nodiscard]] constexpr T ValueOrElse(F&& f) const &
        {
            static_assert(std::same_as<T, std::invoke_result_t<F>>,
                          "F must return type T!");
            return HasValue() ? **this : std::invoke(std::forward<F>(f));
        }

        template<typename F>
        requires std::invocable<F>
        [[nodiscard]] constexpr T ValueOrElse(F&& f) &&
        {
            static_assert(std::same_as<T, std::invoke_result_t<F>>,
                          "F must return type T!");
            return HasValue() ? std::move(**this) : std::invoke(std::forward<F>(f));
        }

        constexpr void Reset() noexcept
        {
            m_value = nullptr;
        }

    private:
        T *m_value;
    };

    namespace INTERNAL
    {
        //SFINAE guard for std::hash<> specialization
        template<typename Key, typename Hash>
        concept HashRequirements = std::is_copy_constructible_v<Hash> &&
                                   std::is_move_constructible_v<Hash> &&
                                   std::same_as<std::size_t, std::invoke_result_t<Hash, Key>>;

        template<typename Key, typename Hash = std::hash<Key>>
        concept EnabledHash = HashRequirements<Key, Hash> &&
                              std::is_default_constructible_v<Hash>;

        template<typename T, typename>
        using EnableHashImpl = T;

        template<typename T, typename Key>
        using EnableHash = EnableHashImpl<T, std::enable_if_t<EnabledHash<Key>>>;
    }
}

namespace std
{
    /// @brief Overloads the std::swap algorithm for TRAP::Optional.
    ///        Exchanges the state of lhs with that of rhs.
    ///        Effectively calls lhs.Swap(rhs).
    /// @param lhs Optional object whose state to swap.
    /// @param rhs Optional object whose state to swap.
    template<typename T, std::enable_if_t<std::is_move_constructible_v<T>>* = nullptr,
                         std::enable_if_t<std::is_swappable_v<T>>* = nullptr>
    constexpr void Swap(TRAP::Optional<T>& lhs, TRAP::Optional<T>& rhs) noexcept(noexcept(lhs.Swap(rhs)))
    {
        return lhs.Swap(rhs);
    }

    /// @brief The template specialization of std::hash for the TRAP::Optional class allows
    ///        users to obtain hashes of the values contained in optional objects.
    ///
    ///        The specialization std::hash<TRAP::Optional<T>> is enabled if
    ///        std::hash<std::remove_const_t<T>> is enabled, and is disabled otherwise.
    ///
    ///        When enabled, for an object o of type TRAP::Optional<T> that contains a value,
    ///        std::hash<TRAP::Optional<T>>()(o) evaluates to the same value as
    ///        std::hash<std::remove_const_t<T>>()(*o).
    ///        For an optional that does not contain a value, the hash is unspecified.
    ///
    ///        The member functions of this specialization are not guaranteed to be noexcept
    ///        because the hash of the underlying type might throw.
    /// @tparam T The type of the value contained in optional object.
    template<typename T>
    struct hash<TRAP::INTERNAL::EnableHash<TRAP::Optional<T>, std::remove_const_t<T>>>
    {
        constexpr ::std::size_t operator()(const TRAP::Optional<T> &o) const
        {
            if (!o.HasValue())
                return 0;

            return std::hash<std::remove_const_t<T>>()(*o);
        }
    };
}

#ifdef _MSC_VER
    #pragma warning( pop )
#endif /*_MSC_VER*/

#endif /*TRAP_OPTIONAL_H*/

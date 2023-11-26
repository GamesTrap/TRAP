/// Expected - An implementation of std::expected with extensions
/// Originally written in 2017 by Sy Brand (tartanllama@gmail.com, @TartanLlama)
/// Modified by Jan "GamesTrap" Schuerkamp

#include <initializer_list>
#include <exception>
#include <functional>
#include <type_traits>
#include <utility>
#include <variant>

#include "Core/Base.h"
#include "Core/Types.h"
#include "TRAP_Assert.h"

namespace TRAP
{
    template<typename T, typename E>
    class Expected;

    template<typename E>
    class Unexpected;

    template<typename E>
    class BadExpectedAccess;

    /// @brief BadExpectedAccess<void> is the base class of all other BadExpectedAccess specializations.
    template<>
    class BadExpectedAccess<void> : public std::exception
    {
    public:
        /// @brief Returns the explanatory string.
        /// @return Pointer to a null-terminated string with explanatory information.
        [[nodiscard]] const char* what() const noexcept override
        {
            return "Bad access to TRAP::Expected without expected value";
        }

    protected:
        BadExpectedAccess() noexcept = default;
        constexpr BadExpectedAccess(const BadExpectedAccess&) = default;
        constexpr BadExpectedAccess(BadExpectedAccess&&) = default;
        BadExpectedAccess& operator=(const BadExpectedAccess&) = default;
        BadExpectedAccess& operator=(BadExpectedAccess&&) = default;
        ~BadExpectedAccess() override = default;
    };

    /// @brief Defines a type of object to be thrown by TRAP::Expected::Value() when accessing
    ///        an expected object that contains an unexpected value. BadExpectedAccess<E>
    ///        stores a copy of the unexpected value.
    template<typename E>
    class BadExpectedAccess : public BadExpectedAccess<void>
    {
    public:
        /// @brief Constructs a new BadExpectedAccess<E> object.
        ///        Initializes the stored value with std::move(e).
        constexpr explicit BadExpectedAccess(E e)
            : m_value(std::move(e))
        {}

        /// @brief Returns a reference to the stored value.
        /// @return Reference to the stored value.
        [[nodiscard]] constexpr E& Error() & noexcept
        {
            return m_value;
        }
        /// @brief Returns a reference to the stored value.
        /// @return Reference to the stored value.
        [[nodiscard]] constexpr const E& Error() const& noexcept
        {
            return m_value;
        }
        /// @brief Returns a reference to the stored value.
        /// @return Reference to the stored value.
        [[nodiscard]] constexpr E&& Error() && noexcept
        {
            return std::move(m_value);
        }
        /// @brief Returns a reference to the stored value.
        /// @return Reference to the stored value.
        [[nodiscard]] constexpr const E&& Error() const&& noexcept
        {
            return std::move(m_value);
        }

    private:
        E m_value;
    };

    namespace INTERNAL
    {
        template<typename T>
        constexpr bool IsExpected = false;
        template<typename T, typename Err>
        constexpr bool IsExpected<Expected<T, Err>> = true;

        template<typename T>
        constexpr bool IsUnexpected = false;
        template<typename T>
        constexpr bool IsUnexpected<Unexpected<T>> = true;

        template<typename Fn, typename T>
        using Result = std::remove_cvref_t<std::invoke_result_t<Fn&&, T&&>>;
        template<typename Fn, typename T>
        using ResultXForm = std::remove_cv_t<std::invoke_result_t<Fn&&, T&&>>;
        template<typename Fn>
        using Result0 = std::remove_cvref_t<std::invoke_result_t<Fn&&>>;
        template<typename Fn>
        using Result0XForm = std::remove_cv_t<std::invoke_result_t<Fn&&>>;

        template<typename Err>
        concept CanBeUnexpected = std::is_object_v<Err> && (!std::is_array_v<Err>) &&
                                  (!INTERNAL::IsExpected<Err>) && (!std::is_const_v<Err>) &&
                                  (!std::is_volatile_v<Err>);

        //Tag types for in-place construction from an invocation result.
        struct InPlaceInv{};
        struct UnexpectInv{};
    }

    /// @brief The class template TRAP::Unexpected represents an unexpected value stored in TRAP::Expected.
    ///        In particular, TRAP::Expected has constructors with TRAP::Unexpected as a single argument,
    ///        which creates an expected object that contains an unexpected value.
    ///
    ///        A program is ill-formed if it instantiates an unexpected with a non-object type, an array type,
    ///        a specialization of TRAP::Unexpected, or a cv-qualified type.
    /// @tparam E The type of the unexpected value. The type must not be an array type, a non-object type,
    ///         a specialization of TRAP::Unexpected, or a cv-qualified type.
    template<typename E>
    class Unexpected
    {
    public:
        static_assert(INTERNAL::CanBeUnexpected<E>);

        /// @brief Constructor. Constructs a TRAP::Unexpected object.
        consteval Unexpected() = delete;
        /// @brief Destructor. Destroys a TRAP::Unexpected object.
        constexpr ~Unexpected() = default;

        /// @brief Copy constructor. Constructs a TRAP::Unexpected object.
        ///        Copies the stored value.
        constexpr Unexpected(const Unexpected&) = default;
        /// @brief Move constructor. Constructs a TRAP::Unexpected object.
        ///        Moves the stored value.
        constexpr Unexpected(Unexpected&&) = default;
        /// @brief Copy assignment operator.
        /// @return *this.
        constexpr Unexpected& operator=(const Unexpected&) = default;
        /// @brief Move assignment operator.
        /// @return *this.
        constexpr Unexpected& operator=(Unexpected&&) = default;

        /// @brief Constructs a TRAP::Unexpected object.
        ///        Constructs the stored value, as if by direct-initializing a value of
        ///        type E from std::forward<Err>(e).
        /// @param e Value with which to initialize the contained value.
        template<typename Err = E>
        requires (!std::same_as<std::remove_cvref_t<Err>, Unexpected> &&
                  !std::same_as<std::remove_cvref_t<Err>, std::in_place_t> &&
                  std::is_constructible_v<E, Err>)
        constexpr explicit Unexpected(Err&& e) noexcept(std::is_nothrow_constructible_v<E, Err>)
            : m_value(std::forward<Err>(e))
        {}

        /// @brief Constructs a TRAP::Unexpected object.
        ///        Constructs the stored value, as if by direct-initializing a value of
        ///        type E from the arguments std::forward<Args>(args)....
        /// @param args Arguments with which to initialize the contained value.
        template<typename... Args>
        requires (std::is_constructible_v<E, Args...>)
        constexpr explicit Unexpected([[maybe_unused]] const std::in_place_t _, Args&&... args) noexcept(std::is_nothrow_constructible_v<E, Args...>)
            : m_value(std::forward<Args>(args)...)
        {}

        /// @brief Constructs a TRAP::Unexpected object.
        ///        Constructs the stored value, as if by direct-initializing a value of type E from
        ///        the arguments il, std::forward<Args>(args)....
        /// @param il Initializer list with which to initialize the contained value.
        /// @param args Arguments with which to initialize the contained value.
        template<typename U, typename... Args>
        requires (std::is_constructible_v<E, std::initializer_list<U>&, Args...>)
        constexpr explicit Unexpected([[maybe_unused]] const std::in_place_t _, std::initializer_list<U> il, Args&&... args)
            : m_value(il, std::forward<Args>(args)...)
        {}

        /// @brief Retrieve a reference to the stored value.
        /// @return Reference to the stored value.
        [[nodiscard]] constexpr const E& Error() const& noexcept
        {
            return m_value;
        }
        /// @brief Retrieve a reference to the stored value.
        /// @return Reference to the stored value.
        [[nodiscard]] constexpr E& Error() & noexcept
        {
            return m_value;
        }
        /// @brief Retrieve a reference to the stored value.
        /// @return Reference to the stored value.
        [[nodiscard]] constexpr const E&& Error() const&& noexcept
        {
            return std::move(m_value);
        }
        /// @brief Retrieve a reference to the stored value.
        /// @return Reference to the stored value.
        [[nodiscard]] constexpr E&& Error() && noexcept
        {
            return std::move(m_value);
        }

        /// @brief Swaps the stored values, as if by using std::swap; swap(Error(), other.Error());.
        ///        The program is ill-formed if std::is_swappable_v<E> is false.
        constexpr void Swap(Unexpected& other) noexcept(std::is_nothrow_swappable_v<E>)
        requires std::is_swappable_v<E>
        {
            using std::swap;

            swap(m_value, other.m_value);
        }

        /// @brief Compares the stored values, as if by return x.Error() == y.Error().
        ///        If the expression x.Error() == e.Error() is not well-formed, or if its result
        ///        is not convertible to bool, the program is ill-formed.
        ///
        /// @note This function is not visible to ordinary unqualified or qualified lookup,
        ///       and can only be found by argument-dependent lookup when TRAP::Unexpected<E> is
        ///       an associated class of the arguments.
        template<typename E2>
        [[nodiscard]] friend constexpr bool operator==(const Unexpected& x, const TRAP::Unexpected<E2>& y)
        {
            return x.Error() == y.Error();
        }

        /// @brief Compares the stored values, as if by return x.Error() != y.Error().
        ///        If the expression x.Error() != e.Error() is not well-formed, or if its result
        ///        is not convertible to bool, the program is ill-formed.
        ///
        /// @note This function is not visible to ordinary unqualified or qualified lookup,
        ///       and can only be found by argument-dependent lookup when TRAP::Unexpected<E> is
        ///       an associated class of the arguments.
        template<typename E2>
        [[nodiscard]] friend constexpr bool operator!=(const Unexpected& x, const TRAP::Unexpected<E2>& y)
        {
            return x.Error() != y.Error();
        }

        /// @brief Equivalent to x.Swap(y);.
        /// @note This function is not visible to ordinary unqualified or qualified lookup, and
        ///       can only be found by argument-dependent lookup when TRAP::Unexpected<E> is an
        ///       associated class of the arguments.
        friend constexpr void swap(Unexpected& lhs, Unexpected& rhs) noexcept(noexcept(lhs.Swap(rhs)))
        requires requires {lhs.Swap(rhs);}
        {
            lhs.Swap(rhs);
        }

    private:
        E m_value;
    };

    /// @brief The deduction guide is provided for unexpected to allow deduction from the constructor argument.
    template<typename E>
    Unexpected(E) -> Unexpected<E>;

    /// @brief Create an Unexpected from e, deducing the return type.
    /// @param e Value for the Unexpected.
    /// @return Unexpected with deduced type.
    template<typename E>
    [[nodiscard]] constexpr Unexpected<typename std::decay_t<E>> MakeUnexpected(E&& e)
    {
        return Unexpected<typename std::decay_t<E>>(std::forward<E>(e));
    }

    /// @brief A tag type for in-place construction of an unexpected value in an TRAP::Expected object.
    /// @note Like other construction tag types, Unexpect_t is a trivial, empty class with
    ///       an explicit default constructor.
    struct Unexpect_t
    {
        consteval Unexpect_t() = default;
    };

    /// @brief A constant of type const TRAP::Unexpect_t which is usually directly passed to a constructor
    ///        of TRAP::Expected to construct an unexpected value.
    inline constexpr Unexpect_t Unexpect{};

    namespace INTERNAL
    {
        template<typename T>
        struct Guard
        {
            static_assert(std::is_nothrow_move_constructible_v<T>);

            constexpr explicit Guard(T& x)
                : m_guarded(std::addressof(x)), m_tmp(std::move(x))
            {
                std::destroy_at(m_guarded);
            }

            constexpr ~Guard()
            {
                if(m_guarded) [[unlikely]]
                {
                    std::construct_at(m_guarded, std::move(m_tmp));
                }
            }

            constexpr Guard(Guard&&) = default;
            constexpr Guard& operator=(Guard&&) = default;

            consteval Guard(const Guard&) = delete;
            consteval Guard& operator=(const Guard&) = delete;

            constexpr T&& Release() noexcept
            {
                m_guarded = nullptr;
                return std::move(m_tmp);
            }

        private:
            T* m_guarded;
            T m_tmp;
        };

        template<typename T, typename U, typename V>
        constexpr void ReInit(T* newValue, U* oldValue, V&& arg) noexcept(std::is_nothrow_constructible_v<T, V>)
        {
            if constexpr(std::is_nothrow_constructible_v<T, V>)
            {
                std::destroy_at(oldValue);
                std::construct_at(newValue, std::forward<V>(arg));
            }
            else if constexpr(std::is_nothrow_move_constructible_v<T>)
            {
                T tmp(std::forward<V>(arg));
                std::destroy_at(oldValue);
                std::construct_at(newValue, std::move(tmp));
            }
            else
            {
                Guard<U> guard(*oldValue);
                std::construct_at(newValue, std::forward<V>(arg));
                guard.Release();
            }
        }

        template<typename E>
        constexpr void ThrowOrAbort(E&& e)
        {
#if defined(__EXCEPTIONS) || defined(_HAS_EXCEPTIONS) || defined(__cpp_exceptions)
            throw std::forward<E>(e);
#else
            std::abort();
#endif
        }
    }

    /// @brief The class template TRAP:Expected provides a way to store either of two values.
    ///        An object of TRAP::Expected at any given time either holds an expected value of type T,
    ///        or an unexpected value of type E. TRAP::Expected is never valueless.
    ///
    ///        The stored value is allocated directly within the storage occupied by the expected object.
    ///        No dynamic memory allocation takes place.
    ///
    ///        A program is ill-formed if it instantiates an expected with a reference type, a function type, or
    ///        a specialization of TRAP::Unexpected. In addition, T must not be std::in_place_t or TRAP::Unexpect_t.
    /// @tparam T The type of the expected value. The type must either be (possibly cv-qualified) void, or meet the
    ///         Destructible requirements (in particular, array and reference types are not allowed).
    /// @tparam E The type of the unexpected value. The type must meet the Destructible requirements, and must be
    ///           a valid template argument for TRAP::Unexpected (in particular, arrays, non-object types, and
    ///           cv-qualified types are not allowed).
    template<typename T, typename E>
    class Expected
    {
        static_assert(!std::is_reference_v<T>, "T must not be a reference!");
        static_assert(!std::is_function_v<T>, "T must not be a function type!");
        static_assert(!std::same_as<std::remove_cv_t<T>, std::in_place_t>, "T must not be std::in_place_t!");
        static_assert(!std::same_as<std::remove_cv_t<T>, Unexpect_t>, "T must not be Unexpect_t!");
        static_assert(!INTERNAL::IsUnexpected<std::remove_cv_t<T>>, "T must not be a specialization of Unexpected!");

        static_assert(INTERNAL::CanBeUnexpected<E>);

        template<typename U, typename Err, typename Unex = Unexpected<E>>
        static constexpr bool ConstructFromExpected = std::disjunction_v<std::is_constructible<T, Expected<U, Err>&>,
                                                                         std::is_constructible<T, Expected<U, Err>>,
                                                                         std::is_constructible<T, const Expected<U, Err>&>,
                                                                         std::is_constructible<T, const Expected<U, Err>>,
                                                                         std::is_convertible<Expected<U, Err>&, T>,
                                                                         std::is_convertible<Expected<U, Err>, T>,
                                                                         std::is_convertible<const Expected<U, Err>&, T>,
                                                                         std::is_convertible<const Expected<U, Err>, T>,
                                                                         std::is_constructible<Unex, Expected<U, Err>&>,
                                                                         std::is_constructible<Unex, Expected<U, Err>>,
                                                                         std::is_constructible<Unex, const Expected<U, Err>&>,
                                                                         std::is_constructible<Unex, const Expected<U, Err>>>;

        template<typename U, typename Err>
        static constexpr bool ExplicitConvertible = std::disjunction_v<std::negation<std::is_convertible<U, T>>, std::negation<std::is_convertible<Err, E>>>;

        template<typename U>
        static constexpr bool SameValue = std::same_as<typename U::value_type, T>;

        template<typename U>
        static constexpr bool SameError = std::same_as<typename U::error_type, E>;

    public:
        using value_type = T;
        using error_type = E;
        using unexpected_type = TRAP::Unexpected<E>;

        template<typename U>
        using rebind = TRAP::Expected<U, error_type>;

        /// @brief Constructs a new expected object.
        ///        Default constructor. If T is not (possibly cv-qualified) void,
        ///        constructs an object that contains an expected value, which is
        ///        value-initialized.
        ///
        ///        After construction, HasValue() returns true.
        constexpr Expected() noexcept(std::is_nothrow_default_constructible_v<T>)
        requires std::is_default_constructible_v<T>
            : m_value(), m_hasValue(true)
        {}

        /// @brief Constructs a new expected object.
        ///        Copy constructor. If other.HasValue() is false, the new object contains an
        ///        unexpected value, which is direct-initialized from other.Error(). Otherwise, if T
        ///        is not (possibly cv-qualified) void, the new object contains an expected value, which
        ///        is direct-initialized from *other.
        ///
        ///        After construction, HasValue() is equal to other.HasValue().
        /// @param other Another expected object whose contained value is copied.
        constexpr Expected(const Expected& other) = default;

        /// @brief Constructs a new expected object.
        ///        Copy constructor. If other.HasValue() is false, the new object contains an
        ///        unexpected value, which is direct-initialized from other.Error(). Otherwise, if T
        ///        is not (possibly cv-qualified) void, the new object contains an expected value, which
        ///        is direct-initialized from *other.
        ///
        ///        After construction, HasValue() is equal to other.HasValue().
        /// @param other Another expected object whose contained value is copied.
        constexpr Expected(const Expected& other) noexcept(std::conjunction_v<std::is_nothrow_copy_constructible<T>, std::is_nothrow_copy_constructible<E>>)
        requires std::is_copy_constructible_v<T> && std::is_copy_constructible_v<E> &&
                 (!std::is_trivially_copy_constructible_v<T> || !std::is_trivially_copy_constructible_v<E>)
            : m_hasValue(other.m_hasValue)
        {
            if(m_hasValue)
                std::construct_at(std::addressof(m_value), other.m_value);
            else
                std::construct_at(std::addressof(m_unexpected), other.m_unexpected);
        }

        /// @brief Constructs a new expected object.
        ///        Move constructor. If other.HasValue() is false, the new object contains an
        ///        unexpected value, which is direct-initialized from std::move(other.Error()).
        ///        Otherwise, if T is not (possibly cv-qualified) void, the new object contains an
        ///        expected value, which is direct-initialized from std::move(*other).
        ///
        ///        After construction, HasValue() is equal to other.HasValue().
        /// @param other Another expected object whose contained value is copied.
        constexpr Expected(Expected&& other) = default;

        /// @brief Constructs a new expected object.
        ///        Move constructor. If other.HasValue() is false, the new object contains an
        ///        unexpected value, which is direct-initialized from std::move(other.Error()).
        ///        Otherwise, if T is not (possibly cv-qualified) void, the new object contains an
        ///        expected value, which is direct-initialized from std::move(*other).
        ///
        ///        After construction, HasValue() is equal to other.HasValue().
        /// @param other Another expected object whose contained value is copied.
        constexpr Expected(Expected&& other) noexcept(std::conjunction_v<std::is_nothrow_move_constructible<T>, std::is_nothrow_move_constructible<E>>)
        requires std::is_move_constructible_v<T> && std::is_move_constructible_v<E> &&
                 (!std::is_trivially_move_constructible_v<T> || !std::is_trivially_move_constructible_v<E>)
            : m_hasValue(other.m_hasValue)
        {
            if(m_hasValue)
                std::construct_at(std::addressof(m_value), std::move(other).m_value);
            else
                std::construct_at(std::addressof(m_unexpected), std::move(other).m_unexpected);
        }

        /// @brief Constructs a new expected object.
        ///        Let
        ///            - UF be std::add_lvalue_reference_t<const U>, and
        ///            - GF be const G&.
        ///
        ///        If other.HasValue() is false, the new object contains an unexpected value, which is
        ///        direct-initialized from std::forward<GF>(other.Error()). Otherwise, if T is not
        ///        (possibly cv-qualified) void, the new object contains an expected value,
        ///        which is direct-initialized from std::forward<UF>(*other). After construction,
        ///        HasValue() is equal to other.HasValue().
        /// @param other Another expected object whose contained value is copied.
        template<typename U, typename G>
        requires std::is_constructible_v<T, const U&> && std::is_constructible_v<E, const G&> &&
                 (!ConstructFromExpected<U, G>)
        constexpr explicit(ExplicitConvertible<const U&, const G&>) Expected(const Expected<U, G>& other) noexcept(std::conjunction_v<std::is_nothrow_constructible<T, const U>,
                                                                                                                                      std::is_nothrow_constructible<E, const G&>>)
            : m_hasValue(other.m_hasValue)
        {
            if(m_hasValue)
                std::construct_at(std::addressof(m_value), other.m_value);
            else
                std::construct_at(std::addressof(m_unexpected), other.m_unexpected);
        }

        /// @brief Constructs a new expected object.
        ///        Let
        ///            - UF be U, and
        ///            - GF be G.
        ///
        ///        If other.HasValue() is false, the new object contains an unexpected value, which is
        ///        direct-initialized from std::forward<GF>(other.Error()). Otherwise, if T is not
        ///        (possibly cv-qualified) void, the new object contains an expected value,
        ///        which is direct-initialized from std::forward<UF>(*other). After construction,
        ///        HasValue() is equal to other.HasValue().
        /// @param other Another expected object whose contained value is copied.
        template<typename U, typename G>
        requires std::is_constructible_v<T, U> && std::is_constructible_v<E, G> &&
                 (!ConstructFromExpected<U, G>)
        constexpr explicit(ExplicitConvertible<U, G>) Expected(Expected<U, G>&& other) noexcept(std::conjunction_v<std::is_nothrow_constructible<T, U>,
                                                                                                                   std::is_nothrow_constructible<E, G>>)
            : m_hasValue(other.m_hasValue)
        {
            if(m_hasValue)
                std::construct_at(std::addressof(m_value), std::move(other).m_value);
            else
                std::construct_at(std::addressof(m_unexpected), std::move(other).m_unexpected);
        }

        /// @brief Constructs a new expected object.
        ///        Constructs an object that contains an expected value, initialized as if direct-initializing
        ///        (but not direct-list-initializing) an object of type T with the expression
        ///        std::forward<U>(v).
        ///
        ///        After construction, HasValue() returns true.
        /// @param v Value with which to initialize the contained value.
        template<typename U = T>
        requires (!std::same_as<std::remove_cvref_t<U>, Expected>) &&
                 (!std::same_as<std::remove_cvref_t<U>, std::in_place_t>) &&
                 (!INTERNAL::IsUnexpected<std::remove_cvref_t<U>>) &&
                 std::is_constructible_v<T, U>
        constexpr explicit(!std::is_convertible_v<U, T>) Expected(U&& v) noexcept(std::is_nothrow_constructible_v<T, U>)
            : m_value(std::forward<U>(v)), m_hasValue(true)
        {}

        /// @brief Constructs a new expected object.
        ///        Let GF be const G&.
        ///
        ///        Constructs an object that contains an unexpected value, which is direct-initialized from
        ///        std::forward<GF>(e.Error()). After construction, HasValue() returns false.
        /// @param u TRAP::Unexpected object whose contained value is copied.
        template<typename G = E>
        requires std::is_constructible_v<E, const G&>
        constexpr explicit(!std::is_convertible_v<const G&, E>) Expected(const Unexpected<G>& u) noexcept(std::is_nothrow_constructible_v<E, const G&>)
            : m_unexpected(u.Error()), m_hasValue(false)
        {}

        /// @brief Constructs a new expected object.
        ///        Let GF be G.
        ///
        ///        Constructs an object that contains an unexpected value, which is direct-initialized from
        ///        std::forward<GF>(e.Error()). After construction, HasValue() returns false.
        /// @param u TRAP::Unexpected object whose contained value is copied.
        template<typename G = E>
        requires std::is_constructible_v<E, G>
        constexpr explicit(!std::is_convertible_v<G, E>) Expected(Unexpected<G>&& u) noexcept(std::is_nothrow_constructible_v<E, G>)
            : m_unexpected(std::move(u).Error()), m_hasValue(false)
        {}

        /// @brief Constructs a new expected object.
        ///        Constructs an object that contains an expected value, which is direct-initialized
        ///        from the arguments std::forward<Args>(args)....
        ///
        ///        After construction, HasValue() return true.
        /// @param args Arguments with which to initialize the contained value.
        template<typename... Args>
        requires std::is_constructible_v<T, Args...>
        constexpr explicit Expected([[maybe_unused]] std::in_place_t _, Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
            : m_value(std::forward<Args>(args)...), m_hasValue(true)
        {}

        /// @brief Constructs a new expected object.
        ///        Constructs an object that contains an expected value, which is direct-initialized
        ///        from the arguments il, std::forward<Args>(args)....
        ///
        ///        After construction, HasValue() return true.
        /// @param il Initializer list with which to initialize the contained value.
        /// @param args Arguments with which to initialize the contained value.
        template<typename U, typename... Args>
        requires std::is_constructible_v<T, std::initializer_list<U>&, Args...>
        constexpr explicit Expected([[maybe_unused]] std::in_place_t _, std::initializer_list<U> il, Args&&... args) noexcept(std::is_nothrow_constructible_v<T, std::initializer_list<U>&, Args...>)
            : m_value(il, std::forward<Args>(args)...), m_hasValue(true)
        {}

        /// @brief Constructs a new expected object.
        ///        Constructs an object that contains an unexpected value, which is
        ///        direct-initialized from the arguments std::forward<Args>(args)....
        ///
        ///        After construction, HasValue() returns false.
        /// @param args Arguments with which to initialize the contained value.
        template<typename... Args>
        requires std::is_constructible_v<E, Args...>
        constexpr explicit Expected([[maybe_unused]] Unexpect_t _, Args&&... args) noexcept(std::is_nothrow_constructible_v<E, Args...>)
            : m_unexpected(std::forward<Args>(args)...), m_hasValue(false)
        {}

        /// @brief Constructs a new expected object.
        ///        Constructs an object that contains an unexpected value, which is
        ///        direct-initialized from the arguments il, std::forward<Args>(args)....
        ///
        ///        After construction, HasValue() returns false.
        /// @param il Initializer list with which to initialize the contained value.
        /// @param args Arguments with which to initialize the contained value.
        template<typename U, typename... Args>
        requires std::is_constructible_v<E, std::initializer_list<U>&, Args...>
        constexpr explicit Expected([[maybe_unused]] Unexpect_t _, std::initializer_list<U> il, Args&&... args) noexcept(std::is_nothrow_constructible_v<E, std::initializer_list<U>&, Args...>)
            : m_unexpected(il, std::forward<Args>(args)...), m_hasValue(false)
        {}

        /// @brief Destroys the currently contained value. Thas if, if HasValue() is false,
        ///        destroys the unexpected value; otherwise, if T is not (possibly cv-qualified)
        ///        void, destroys the expected value.
        constexpr ~Expected() = default;

        /// @brief Destroys the currently contained value. Thas if, if HasValue() is false,
        ///        destroys the unexpected value; otherwise, if T is not (possibly cv-qualified)
        ///        void, destroys the expected value.
        constexpr ~Expected()
        requires (!std::is_trivially_destructible_v<T>) || (!std::is_trivially_destructible_v<E>)
        {
            if(m_hasValue)
                std::destroy_at(std::addressof(m_value));
            else
                std::destroy_at(std::addressof(m_unexpected));
        }

        /// @brief Assigns a new value to an existing expected object.
        ///        Assigns the state of other.
        ///        - If this->HasValue() equals other.HasValue(), assigns the value contained in other.
        ///          Does nothing if T is (possibly cv-qualified) void and other.HasValue() is true.
        ///        - Otherwise, destroys the currently contained value (does nothing if this->HasValue()
        ///          is true and T is (possibly cv-qualified) void), and makes *this contain a copy of
        ///          the value contained in other.
        ///
        ///          If other.HasValue() is true and T is (possibly cv-qualified) void, does not construct
        ///          the new value. Otherwise, the new value is copy-constructed from *other.
        ///          If an exception is thrown, the old value is retained; *this does not become valueless.
        ///
        ///        If no exception was thrown, after assignment, HasValue() is equal to other.HasValue().
        /// @param other Another expected object whose contained value to assign.
        /// @return *this.
        constexpr Expected& operator=(const Expected& other) = delete;
        /// @brief Assigns a new value to an existing expected object.
        ///        Assigns the state of other.
        ///        - If this->HasValue() equals other.HasValue(), assigns the value contained in other.
        ///          Does nothing if T is (possibly cv-qualified) void and other.HasValue() is true.
        ///        - Otherwise, destroys the currently contained value (does nothing if this->HasValue()
        ///          is true and T is (possibly cv-qualified) void), and makes *this contain a copy of
        ///          the value contained in other.
        ///
        ///          If other.HasValue() is true and T is (possibly cv-qualified) void, does not construct
        ///          the new value. Otherwise, the new value is copy-constructed from *other.
        ///          If an exception is thrown, the old value is retained; *this does not become valueless.
        ///
        ///        If no exception was thrown, after assignment, HasValue() is equal to other.HasValue().
        /// @param other Another expected object whose contained value to assign.
        /// @return *this.
        constexpr Expected& operator=(const Expected& other) noexcept(std::conjunction_v<std::is_nothrow_copy_constructible<T>,
                                                                                         std::is_nothrow_copy_constructible<E>,
                                                                                         std::is_nothrow_copy_assignable<T>,
                                                                                         std::is_nothrow_copy_assignable<E>>)
        requires std::is_copy_assignable_v<T> && std::is_copy_constructible_v<T> &&
                 std::is_copy_assignable_v<E> && std::is_copy_constructible_v<E> &&
                 (std::is_nothrow_move_constructible_v<T> || std::is_nothrow_move_constructible_v<E>)
        {
            if(other.m_hasValue)
                this->AssignValue(other.m_value);
            else
                this->AssignUnexpected(other.m_unexpected);

            return *this;
        }

        /// @brief Assigns a new value to an existing expected object.
        ///        Assigns the state of other.
        ///        - If this->HasValue() equals other.HasValue(), assigns the value contained in other.
        ///          Does nothing if T is (possibly cv-qualified) void and other.HasValue() is true.
        ///        - Otherwise, destroys the currently contained value (does nothing if this->HasValue()
        ///          is true and T is (possibly cv-qualified) void), and makes *this contain a copy of
        ///          the value contained in other.
        ///
        ///          If other.HasValue() is true and T is (possibly cv-qualified) void, does not construct
        ///          the new value. Otherwise, the new value is move-constructed from other.Error(), as
        ///          appropriate. If an exception is thrown, the old value is retained; *this does not become
        ///          valueless.
        ///
        ///        If no exception was thrown, after assignment, HasValue() is equal to other.HasValue().
        /// @param other Another expected object whose contained value to assign.
        /// @return *this.
        constexpr Expected& operator=(Expected&& other) noexcept(std::conjunction_v<std::is_nothrow_move_constructible<T>,
                                                                                    std::is_nothrow_move_constructible<E>,
                                                                                    std::is_nothrow_move_assignable<T>,
                                                                                    std::is_nothrow_move_assignable<E>>)
        requires std::is_move_assignable_v<T> && std::is_move_constructible_v<T> &&
                 std::is_move_assignable_v<E> && std::is_move_constructible_v<E> &&
                 (std::is_nothrow_move_constructible_v<T> || std::is_nothrow_move_constructible_v<E>)
        {
            if(other.m_hasValue)
                AssignValue(std::move(other.m_value));
            else
                AssignUnexpected(std::move(other.m_unexpected));

            return *this;
        }

        /// @brief Assigns a new value to an existing expected object.
        ///        Assigns from expected value,
        ///            - If this->HasValue() is true, equivalent to **this = std::forward<U>(v).
        ///            - Otherwise, destroys the value contained in *this, and makes *this contain
        ///              a value initialized from std::forward<U>(v). If an exception is thrown,
        ///              the old value is retained; *this does not become valueless.
        ///
        ///        If no exception was thrown, after assignment, this->HasValue() is true.
        /// @param v Value to assign to the contained value.
        template<typename U = T>
        requires (!std::same_as<Expected, std::remove_cvref_t<U>>) &&
                 (!INTERNAL::IsUnexpected<std::remove_cvref_t<U>>) &&
                 std::is_constructible_v<T, U> && std::is_assignable_v<T&, U> &&
                 (std::is_nothrow_constructible_v<T, U> || std::is_nothrow_move_constructible_v<T> ||
                  std::is_nothrow_move_constructible_v<E>)
        constexpr Expected& operator=(U&& v)
        {
            AssignValue(std::forward<U>(v));
            return *this;
        }

        /// @brief Assigns a new value to an existing expected object.
        ///        Assigns from unexpected value.
        ///        Let GF be const G&.
        ///            - If this->HasValue() is true, destroys the value contained in *this
        ///              (does nothing if T is (possibly cv-qualified) void), and makes
        ///              *this contain a value initialized from std::forward<GF>(e.Error()).
        ///              If an exception is thrown, the old value is retained; *this does not
        ///              become valueless.
        ///            - Otherwise, equivalent to this->Error() = std::forward<GF>(e.Error()).
        ///
        ///        If no exception was thrown, after assignment, this->HasValue() is false.
        /// @param e TRAP::Unexpected object whose contained value to assign.
        template<typename G>
        requires std::is_constructible_v<E, const G&> &&
                 std::is_assignable_v<E&, const G&> &&
                 (std::is_nothrow_constructible_v<E, const G&> ||
                  std::is_nothrow_move_constructible_v<T> ||
                  std::is_nothrow_move_constructible_v<E>)
        constexpr Expected& operator=(const Unexpected<G>& e)
        {
            AssignUnexpected(e.Error());
            return *this;
        }

        /// @brief Assigns a new value to an existing expected object.
        ///        Assigns from unexpected value.
        ///        Let GF be G.
        ///            - If this->HasValue() is true, destroys the value contained in *this
        ///              (does nothing if T is (possibly cv-qualified) void), and makes
        ///              *this contain a value initialized from std::forward<GF>(e.Error()).
        ///              If an exception is thrown, the old value is retained; *this does not
        ///              become valueless.
        ///            - Otherwise, equivalent to this->Error() = std::forward<GF>(e.Error()).
        ///
        ///        If no exception was thrown, after assignment, this->HasValue() is false.
        /// @param e TRAP::Unexpected object whose contained value to assign.
        template<typename G>
        requires std::is_constructible_v<E, G> &&
                 std::is_assignable_v<E&, G> &&
                 (std::is_nothrow_constructible_v<E, G> ||
                  std::is_nothrow_move_constructible_v<T> ||
                  std::is_nothrow_move_constructible_v<E>)
        constexpr Expected& operator=(Unexpected<G>&& e)
        {
            AssignUnexpected(std::move(e).Error());
            return *this;
        }

        /// @brief Constructs an expected value in-place. After the call, HasValue() returns true.
        ///        Destroys the contained value, then initializes the expected value contained
        ///        in *this as if by direct-initializing an object of type T from the arguments
        ///        std::forward<Args>(args)....
        /// @param args The arguments to pass to the constructor.
        /// @return A reference to the new contained value.
        template<typename... Args>
        requires std::is_nothrow_constructible_v<T, Args...>
        constexpr T& Emplace(Args&&... args) noexcept
        {
            if(m_hasValue)
                std::destroy_at(std::addressof(m_value));
            else
            {
                std::destroy_at(std::addressof(m_unexpected));
                m_hasValue = true;
            }

            std::construct_at(std::addressof(m_value), std::forward<Args>(args)...);
            return m_value;
        }

        /// @brief Constructs an expected value in-place. After the call, HasValue() returns true.
        ///        Destroys the contained value, then initializes the expected value contained
        ///        in *this as if by direct-initializing an object of type T from the arguments
        ///        il, std::forward<Args>(args)....
        /// @param il The initializer list to pass to the constructor.
        /// @param args The arguments to pass to the constructor.
        /// @return A reference to the new contained value.
        template<typename U, typename... Args>
        requires std::is_nothrow_constructible_v<T, std::initializer_list<U>&, Args...>
        constexpr T& Emplace(std::initializer_list<U> il, Args&&... args) noexcept
        {
            if(m_hasValue)
                std::destroy_at(std::addressof(m_value));
            else
            {
                std::destroy_at(std::addressof(m_unexpected));
                m_hasValue = true;
            }

            std::construct_at(std::addressof(m_value), il, std::forward<Args>(args)...);
            return m_value;
        }

        /// @brief Swaps the contents with those of other.
        /// @param x The expected object to exchange the contents with.
        constexpr void Swap(Expected& x) noexcept(std::conjunction_v<std::is_nothrow_move_constructible<T>,
                                                                     std::is_nothrow_move_constructible<E>,
                                                                     std::is_nothrow_swappable<T&>,
                                                                     std::is_nothrow_swappable<E&>>)
        requires std::is_swappable_v<T> && std::is_swappable_v<E> &&
                 std::is_move_constructible_v<T> && std::is_move_constructible_v<E> &&
                 (std::is_nothrow_move_constructible_v<T> || std::is_nothrow_move_constructible_v<E>)
        {
            if(m_hasValue)
            {
                if(x.m_hasValue)
                {
                    using std::swap;
                    swap(m_value, x.m_value);
                }
                else
                    this->SwapValueUnexpected(x);
            }
            else
            {
                if(x.m_hasValue)
                    x.SwapValueUnexpected(*this);
                else
                {
                    using std::swap;
                    swap(m_unexpected, x.m_unexpected);
                }
            }
        }

        /// @brief Accesses the expected value contained in *this.
        ///        Returns a pointer to the contained value.
        ///
        ///        The behaviour is undefined if this->HasValue() is false.
        /// @return Pointer to the contained value.
        /// @note This operator does not check whether the optional contains a value!
        ///       You can do so manually by using HasValue() or simply operator bool().
        ///       Alternatively, if checked access is needed, Value(), ValueOr() or ValueOrElse() may be used.
        [[nodiscard]] constexpr const T* operator->() const noexcept
        {
            TRAP_ASSERT(m_hasValue);
            return std::addressof(m_value);
        }
        /// @brief Accesses the expected value contained in *this.
        ///        Returns a pointer to the contained value.
        ///
        ///        The behaviour is undefined if this->HasValue() is false.
        /// @return Pointer to the contained value.
        /// @note This operator does not check whether the optional contains a value!
        ///       You can do so manually by using HasValue() or simply operator bool().
        ///       Alternatively, if checked access is needed, Value(), ValueOr() or ValueOrElse() may be used.
        [[nodiscard]] constexpr T* operator->() noexcept
        {
            TRAP_ASSERT(m_hasValue);
            return std::addressof(m_value);
        }

        /// @brief Accesses the expected value contained in *this.
        ///        Returns a reference to the contained value.
        ///
        ///        The behaviour is undefined if this->HasValue() is false.
        /// @return Reference to the contained value.
        /// @note This operator does not check whether the optional contains a value!
        ///       You can do so manually by using HasValue() or simply operator bool().
        ///       Alternatively, if checked access is needed, Value(), ValueOr() or ValueOrElse() may be used.
        [[nodiscard]] constexpr const T& operator*() const& noexcept
        {
            TRAP_ASSERT(m_hasValue);
            return m_value;
        }
        /// @brief Accesses the expected value contained in *this.
        ///        Returns a reference to the contained value.
        ///
        ///        The behaviour is undefined if this->HasValue() is false.
        /// @return Reference to the contained value.
        /// @note This operator does not check whether the optional contains a value!
        ///       You can do so manually by using HasValue() or simply operator bool().
        ///       Alternatively, if checked access is needed, Value(), ValueOr() or ValueOrElse() may be used.
        [[nodiscard]] constexpr T& operator*() & noexcept
        {
            TRAP_ASSERT(m_hasValue);
            return m_value;
        }
        /// @brief Accesses the expected value contained in *this.
        ///        Returns a reference to the contained value.
        ///
        ///        The behaviour is undefined if this->HasValue() is false.
        /// @return Reference to the contained value.
        /// @note This operator does not check whether the optional contains a value!
        ///       You can do so manually by using HasValue() or simply operator bool().
        ///       Alternatively, if checked access is needed, Value(), ValueOr() or ValueOrElse() may be used.
        [[nodiscard]] constexpr const T&& operator*() const&& noexcept
        {
            TRAP_ASSERT(m_hasValue);
            return std::move(m_value);
        }
        /// @brief Accesses the expected value contained in *this.
        ///        Returns a reference to the contained value.
        ///
        ///        The behaviour is undefined if this->HasValue() is false.
        /// @return Reference to the contained value.
        /// @note This operator does not check whether the optional contains a value!
        ///       You can do so manually by using HasValue() or simply operator bool().
        ///       Alternatively, if checked access is needed, Value(), ValueOr() or ValueOrElse() may be used.
        [[nodiscard]] constexpr T&& operator*() && noexcept
        {
            TRAP_ASSERT(m_hasValue);
            return std::move(m_value);
        }

        /// @brief Checks whether *this contains an expected value.
        /// @return true if *this contains an expected value, false if *this contains an unexpected value.
        /// @note A TRAP::Expected object is never empty. If HasValue() returns true,
        ///       operator* can be used to access the contained value; otherwise, Error() can be used.
        [[nodiscard]] constexpr explicit operator bool() const noexcept
        {
            return m_hasValue;
        }
        /// @brief Checks whether *this contains an expected value.
        /// @return true if *this contains an expected value, false if *this contains an unexpected value.
        /// @note A TRAP::Expected object is never empty. If HasValue() returns true,
        ///       operator* can be used to access the contained value; otherwise, Error() can be used.
        [[nodiscard]] constexpr bool HasValue() const noexcept
        {
            return m_hasValue;
        }

        /// @brief If *this contains an expected value, returns a reference to the
        ///        contained value. Returns nothing if T is (possibly cv-qualified) void.
        ///        Otherwise, throws an exception of type TRAP::BadExpectedAccess<std::decay_t<E>>
        ///        that contains a copy of Error().
        /// @return The expected value contained in *this.
        [[nodiscard]] constexpr const T& Value() const&
        {
            if(m_hasValue) [[likely]]
                return m_value;

            INTERNAL::ThrowOrAbort(BadExpectedAccess<E>(m_unexpected));
        }
        /// @brief If *this contains an expected value, returns a reference to the
        ///        contained value. Returns nothing if T is (possibly cv-qualified) void.
        ///        Otherwise, throws an exception of type TRAP::BadExpectedAccess<std::decay_t<E>>
        ///        that contains a copy of Error().
        /// @return The expected value contained in *this.
        [[nodiscard]] constexpr T& Value() &
        {
            if(m_hasValue) [[likely]]
                return m_value;

            const auto& unex = m_unexpected;
            INTERNAL::ThrowOrAbort(BadExpectedAccess<E>(unex));
        }
        /// @brief If *this contains an expected value, returns a reference to the
        ///        contained value. Returns nothing if T is (possibly cv-qualified) void.
        ///        Otherwise, throws an exception of type TRAP::BadExpectedAccess<std::decay_t<E>>
        ///        that contains a copy of Error().
        /// @return The expected value contained in *this.
        [[nodiscard]] constexpr const T&& Value() const&&
        {
            if(m_hasValue) [[likely]]
                return std::move(m_value);

            INTERNAL::ThrowOrAbort(BadExpectedAccess<E>(std::move(m_unexpected)));
        }
        /// @brief If *this contains an expected value, returns a reference to the
        ///        contained value. Returns nothing if T is (possibly cv-qualified) void.
        ///        Otherwise, throws an exception of type TRAP::BadExpectedAccess<std::decay_t<E>>
        ///        that contains a copy of Error().
        /// @return The expected value contained in *this.
        [[nodiscard]] constexpr T&& Value() &&
        {
            if(m_hasValue) [[likely]]
                return std::move(m_value);

            INTERNAL::ThrowOrAbort(BadExpectedAccess<E>(std::move(m_unexpected)));
        }

        /// @brief Accesses the unexpected value contained in *this.
        ///        The behaviour is undefined if this->HasValue() is true.
        /// @return Reference to the unexpected value contained in *this.
        [[nodiscard]] constexpr const E& Error() const& noexcept
        {
            TRAP_ASSERT(!m_hasValue);
            return m_unexpected;
        }
        /// @brief Accesses the unexpected value contained in *this.
        ///        The behaviour is undefined if this->HasValue() is true.
        /// @return Reference to the unexpected value contained in *this.
        [[nodiscard]] constexpr E& Error() & noexcept
        {
            TRAP_ASSERT(!m_hasValue);
            return m_unexpected;
        }
        /// @brief Accesses the unexpected value contained in *this.
        ///        The behaviour is undefined if this->HasValue() is true.
        /// @return Reference to the unexpected value contained in *this.
        [[nodiscard]] constexpr const E&& Error() const&& noexcept
        {
            TRAP_ASSERT(!m_hasValue);
            return std::move(m_unexpected);
        }
        /// @brief Accesses the unexpected value contained in *this.
        ///        The behaviour is undefined if this->HasValue() is true.
        /// @return Reference to the unexpected value contained in *this.
        [[nodiscard]] constexpr E&& Error() && noexcept
        {
            TRAP_ASSERT(!m_hasValue);
            return std::move(m_unexpected);
        }

        /// @brief Returns the contained value if *this contains an expected value,
        ///        otherwise returns v.
        /// @param v The value to use in case *this does not contain an expected value.
        /// @return The currently contained value if *this contains an expected value, or v otherwise.
        template<typename U>
        [[nodiscard]] constexpr T ValueOr(U&& v) const& noexcept(std::conjunction_v<std::is_nothrow_copy_constructible<T>,
                                                                                    std::is_nothrow_convertible<U, T>>)
        {
            static_assert(std::is_copy_constructible_v<T> && std::is_convertible_v<U, T>,
                          "T must be copy-constructible and convertible to from U&&");

            if(m_hasValue)
                return m_value;

            return static_cast<T>(std::forward<U>(v));
        }
        /// @brief Returns the contained value if *this contains an expected value,
        ///        otherwise returns v.
        /// @param v The value to use in case *this does not contain an expected value.
        /// @return The currently contained value if *this contains an expected value, or v otherwise.
        template<typename U>
        [[nodiscard]] constexpr T ValueOr(U&& v) && noexcept(std::conjunction_v<std::is_nothrow_move_constructible<T>,
                                                                                std::is_nothrow_convertible<U, T>>)
        {
            static_assert(std::is_move_constructible_v<T> && std::is_convertible_v<U, T>,
                          "T must be move-constructible and convertible to from U&&");

            if(m_hasValue)
                return std::move(m_value);

            return static_cast<T>(std::forward<U>(v));
        }

        /// @brief Returns the contained value if *this contains an expected value,
        ///        otherwise returns the result of f.
        /// @param f A suitable function or Callable object whose call signature returns a non-reference type.
        /// @return The currently contained value if *this contains an expected value, or result of f otherwise.
        template<typename F>
        requires std::invocable<F>
        [[nodiscard]] constexpr T ValueOrElse(F&& f) const &
        {
            static_assert(std::same_as<T, std::invoke_result_t<F>>,
                          "F must return type T!");

            if(m_hasValue)
                return m_value;

            return std::invoke(std::forward<F>(f));
        }
        /// @brief Returns the contained value if *this contains an expected value,
        ///        otherwise returns the result of f.
        /// @param f A suitable function or Callable object whose call signature returns a non-reference type.
        /// @return The currently contained value if *this contains an expected value, or result of f otherwise.
        template<typename F>
        requires std::invocable<F>
        [[nodiscard]] constexpr T ValueOrElse(F&& f) &&
        {
            static_assert(std::same_as<T, std::invoke_result_t<F>>,
                          "F must return type T!");

            if(m_hasValue)
                return m_value;

            return std::invoke(std::forward<F>(f));
        }

        /// @brief Returns the contained value if *this contains an unexpected value,
        ///        otherwise returns e.
        /// @param e The value to use in case *this does contain an expected value.
        /// @return The currently contained value if *this contains an unexpected value, or e otherwise.
        template<typename G = E>
        [[nodiscard]] constexpr E ErrorOr(G&& e) const&
        {
            static_assert(std::is_copy_constructible_v<E> && std::is_convertible_v<G, E>,
                          "E must be copy-constructible and convertible to from G&&");

            if(m_hasValue)
                return std::forward<G>(e);

            return m_unexpected;
        }
        /// @brief Returns the contained value if *this contains an unexpected value,
        ///        otherwise returns e.
        /// @param e The value to use in case *this does contain an expected value.
        /// @return The currently contained value if *this contains an unexpected value, or e otherwise.
        template<typename G = E>
        [[nodiscard]] constexpr E ErrorOr(G&& e) &&
        {
            static_assert(std::is_move_constructible_v<E> && std::is_convertible_v<G, E>,
                          "E must be move-constructible and convertible to from G&&");

            if(m_hasValue)
                return std::forward<G>(e);

            return std::move(m_unexpected);
        }

        /// @brief If *this contains an expected value, invokes f and returns its result; otherwise,
        ///        returns a TRAP::Expected object that contains a copy of Error().
        ///
        ///        If T is bot (possibly cv-qualified) void, the contained expected value (obtained
        ///        from operator*) is passed as an argument to f; otherwise f takes no argument.
        /// @param f A suitable function or Callable object that returns a TRAP::Expected.
        /// @return The result of f or a TRAP::Expected object that contains an error value, as described above.
        template<typename Fn>
        requires std::is_constructible_v<E, E&>
        [[nodiscard]] constexpr auto AndThen(Fn&& f) &
        {
            using U = INTERNAL::Result<Fn, T&>;
            static_assert(INTERNAL::IsExpected<U>, "The function passed to TRAP::Expected<T, E>::AndThen must return a TRAP::Expected!");
            static_assert(std::same_as<typename U::error_type, E>, "The function passed to TRAP::Expected<T, E>::AndThen must return a TRAP::Expected with the same error_type!");

            if(HasValue())
                return std::invoke(std::forward<Fn>(f), m_value);

            return U(Unexpect, m_unexpected);
        }
        /// @brief If *this contains an expected value, invokes f and returns its result; otherwise,
        ///        returns a TRAP::Expected object that contains a copy of Error().
        ///
        ///        If T is bot (possibly cv-qualified) void, the contained expected value (obtained
        ///        from operator*) is passed as an argument to f; otherwise f takes no argument.
        /// @param f A suitable function or Callable object that returns a TRAP::Expected.
        /// @return The result of f or a TRAP::Expected object that contains an error value, as described above.
        template<typename Fn>
        requires std::is_constructible_v<E, const E&>
        [[nodiscard]] constexpr auto AndThen(Fn&& f) const&
        {
            using U = INTERNAL::Result<Fn, const T&>;
            static_assert(INTERNAL::IsExpected<U>, "The function passed to TRAP::Expected<T, E>::AndThen must return a TRAP::Expected!");
            static_assert(std::same_as<typename U::error_type, E>, "The function passed to TRAP::Expected<T, E>::AndThen must return a TRAP::Expected with the same error_type!");

            if(HasValue())
                return std::invoke(std::forward<Fn>(f), m_value);

            return U(Unexpect, m_unexpected);
        }
        /// @brief If *this contains an expected value, invokes f and returns its result; otherwise,
        ///        returns a TRAP::Expected object that contains a copy of Error().
        ///
        ///        If T is bot (possibly cv-qualified) void, the contained expected value (obtained
        ///        from operator*) is passed as an argument to f; otherwise f takes no argument.
        /// @param f A suitable function or Callable object that returns a TRAP::Expected.
        /// @return The result of f or a TRAP::Expected object that contains an error value, as described above.
        template<typename Fn>
        requires std::is_constructible_v<E, E>
        [[nodiscard]] constexpr auto AndThen(Fn&& f) &&
        {
            using U = INTERNAL::Result<Fn, T&&>;
            static_assert(INTERNAL::IsExpected<U>, "The function passed to TRAP::Expected<T, E>::AndThen must return a TRAP::Expected!");
            static_assert(std::same_as<typename U::error_type, E>, "The function passed to TRAP::Expected<T, E>::AndThen must return a TRAP::Expected with the same error_type!");

            if(HasValue())
                return std::invoke(std::forward<Fn>(f), std::move(m_value));

            return U(Unexpect, std::move(m_unexpected));
        }
        /// @brief If *this contains an expected value, invokes f and returns its result; otherwise,
        ///        returns a TRAP::Expected object that contains a copy of Error().
        ///
        ///        If T is bot (possibly cv-qualified) void, the contained expected value (obtained
        ///        from operator*) is passed as an argument to f; otherwise f takes no argument.
        /// @param f A suitable function or Callable object that returns a TRAP::Expected.
        /// @return The result of f or a TRAP::Expected object that contains an error value, as described above.
        template<typename Fn>
        requires std::is_constructible_v<E, const E>
        [[nodiscard]] constexpr auto AndThen(Fn&& f) const&&
        {
            using U = INTERNAL::Result<Fn, const T&&>;
            static_assert(INTERNAL::IsExpected<U>, "The function passed to TRAP::Expected<T, E>::AndThen must return a TRAP::Expected!");
            static_assert(std::same_as<typename U::error_type, E>, "The function passed to TRAP::Expected<T, E>::AndThen must return a TRAP::Expected with the same error_type!");

            if(HasValue())
                return std::invoke(std::forward<Fn>(f), std::move(m_value));

            return U(Unexpect, std::move(m_unexpected));
        }

        /// @brief If *this contains an uexpected value, invokes f with the argument Error()
        ///        and returns its result; otherwise, returns a TRAP::Expected object that
        ///        contains a copy of the contained expected value (obtained from operator*).
        /// @param f A suitable function or Callable object that returns a TRAP::Expected.
        /// @return The result of f, or a TRAP::Exepected object that contains a copy of the expected value, as described above.
        template<typename Fn>
        requires std::is_constructible_v<T, T&>
        [[nodiscard]] constexpr auto OrElse(Fn&& f) &
        {
            using G = INTERNAL::Result<Fn, E&>;
            static_assert(INTERNAL::IsExpected<G>, "The function passed to TRAP::Expected<T, E>::OrElse must return a TRAP::Expected");
            static_assert(std::same_as<typename G::value_type, T>, "The function passed to TRAP::Expected<T, E>::OrElse must return a TRAP::Expected with the same value_type!");

            if(HasValue())
                return G(std::in_place, m_value);

            return std::invoke(std::forward<Fn>(f), m_unexpected);
        }
        /// @brief If *this contains an uexpected value, invokes f with the argument Error()
        ///        and returns its result; otherwise, returns a TRAP::Expected object that
        ///        contains a copy of the contained expected value (obtained from operator*).
        /// @param f A suitable function or Callable object that returns a TRAP::Expected.
        /// @return The result of f, or a TRAP::Exepected object that contains a copy of the expected value, as described above.
        template<typename Fn>
        requires std::is_constructible_v<T, const T&>
        [[nodiscard]] constexpr auto OrElse(Fn&& f) const&
        {
            using G = INTERNAL::Result<Fn, const E&>;
            static_assert(INTERNAL::IsExpected<G>, "The function passed to TRAP::Expected<T, E>::OrElse must return a TRAP::Expected");
            static_assert(std::same_as<typename G::value_type, T>, "The function passed to TRAP::Expected<T, E>::OrElse must return a TRAP::Expected with the same value_type!");

            if(HasValue())
                return G(std::in_place, m_value);

            return std::invoke(std::forward<Fn>(f), m_unexpected);
        }
        /// @brief If *this contains an uexpected value, invokes f with the argument Error()
        ///        and returns its result; otherwise, returns a TRAP::Expected object that
        ///        contains a copy of the contained expected value (obtained from operator*).
        /// @param f A suitable function or Callable object that returns a TRAP::Expected.
        /// @return The result of f, or a TRAP::Exepected object that contains a copy of the expected value, as described above.
        template<typename Fn>
        requires std::is_constructible_v<T, T>
        [[nodiscard]] constexpr auto OrElse(Fn&& f) &&
        {
            using G = INTERNAL::Result<Fn, E&&>;
            static_assert(INTERNAL::IsExpected<G>, "The function passed to TRAP::Expected<T, E>::OrElse must return a TRAP::Expected");
            static_assert(std::same_as<typename G::value_type, T>, "The function passed to TRAP::Expected<T, E>::OrElse must return a TRAP::Expected with the same value_type!");

            if(HasValue())
                return G(std::in_place, std::move(m_value));

            return std::invoke(std::forward<Fn>(f), std::move(m_unexpected));
        }
        /// @brief If *this contains an uexpected value, invokes f with the argument Error()
        ///        and returns its result; otherwise, returns a TRAP::Expected object that
        ///        contains a copy of the contained expected value (obtained from operator*).
        /// @param f A suitable function or Callable object that returns a TRAP::Expected.
        /// @return The result of f, or a TRAP::Exepected object that contains a copy of the expected value, as described above.
        template<typename Fn>
        requires std::is_constructible_v<T, const T>
        [[nodiscard]] constexpr auto OrElse(Fn&& f) const&&
        {
            using G = INTERNAL::Result<Fn, const E&&>;
            static_assert(INTERNAL::IsExpected<G>, "The function passed to TRAP::Expected<T, E>::OrElse must return a TRAP::Expected");
            static_assert(std::same_as<typename G::value_type, T>, "The function passed to TRAP::Expected<T, E>::OrElse must return a TRAP::Expected with the same value_type!");

            if(HasValue())
                return G(std::in_place, std::move(m_value));

            return std::invoke(std::forward<Fn>(f), std::move(m_unexpected));
        }

        /// @brief If *this contains an expected value, invokes f and returns TRAP::Expected
        ///        object that contains its result; otherwise, returns a TRAP::Expected object
        ///        that contains a copy of Error().
        ///
        ///        If T is not (possibly cv-qualified) void, the contained value (obtained
        ///        from operator*) is passed as an argument to f; otherwise f takes no argument.
        /// @param f A suitable function or Callable object whose call signature returns a non-reference type.
        /// @return A TRAP::Expected object containing either the result of f or an error value, as described above.
        template<typename Fn>
        requires std::is_constructible_v<E, E&>
        [[nodiscard]] constexpr auto Transform(Fn&& f) &
        {
            using U = INTERNAL::ResultXForm<Fn, T&>;
            using Result = Expected<U, E>;

            if(HasValue())
                return Result(INTERNAL::InPlaceInv{}, [&](){return std::invoke(std::forward<Fn>(f), m_value);});

            return Result(Unexpect, m_unexpected);
        }
        /// @brief If *this contains an expected value, invokes f and returns TRAP::Expected
        ///        object that contains its result; otherwise, returns a TRAP::Expected object
        ///        that contains a copy of Error().
        ///
        ///        If T is not (possibly cv-qualified) void, the contained value (obtained
        ///        from operator*) is passed as an argument to f; otherwise f takes no argument.
        /// @param f A suitable function or Callable object whose call signature returns a non-reference type.
        /// @return A TRAP::Expected object containing either the result of f or an error value, as described above.
        template<typename Fn>
        requires std::is_constructible_v<E, const E&>
        [[nodiscard]] constexpr auto Transform(Fn&& f) const&
        {
            using U = INTERNAL::ResultXForm<Fn, const T&>;
            using Result = Expected<U, E>;

            if(HasValue())
                return Result(INTERNAL::InPlaceInv{}, [&](){return std::invoke(std::forward<Fn>(f), m_value);});

            return Result(Unexpect, m_unexpected);
        }
        /// @brief If *this contains an expected value, invokes f and returns TRAP::Expected
        ///        object that contains its result; otherwise, returns a TRAP::Expected object
        ///        that contains a copy of Error().
        ///
        ///        If T is not (possibly cv-qualified) void, the contained value (obtained
        ///        from operator*) is passed as an argument to f; otherwise f takes no argument.
        /// @param f A suitable function or Callable object whose call signature returns a non-reference type.
        /// @return A TRAP::Expected object containing either the result of f or an error value, as described above.
        template<typename Fn>
        requires std::is_constructible_v<E, E>
        [[nodiscard]] constexpr auto Transform(Fn&& f) &&
        {
            using U = INTERNAL::ResultXForm<Fn, T>;
            using Result = Expected<U, E>;

            if(HasValue())
                return Result(INTERNAL::InPlaceInv{}, [&](){return std::invoke(std::forward<Fn>(f), std::move(m_value));});

            return Result(Unexpect, std::move(m_unexpected));
        }
        /// @brief If *this contains an expected value, invokes f and returns TRAP::Expected
        ///        object that contains its result; otherwise, returns a TRAP::Expected object
        ///        that contains a copy of Error().
        ///
        ///        If T is not (possibly cv-qualified) void, the contained value (obtained
        ///        from operator*) is passed as an argument to f; otherwise f takes no argument.
        /// @param f A suitable function or Callable object whose call signature returns a non-reference type.
        /// @return A TRAP::Expected object containing either the result of f or an error value, as described above.
        template<typename Fn>
        requires std::is_constructible_v<E, const E>
        [[nodiscard]] constexpr auto Transform(Fn&& f) const&&
        {
            using U = INTERNAL::ResultXForm<Fn, const T>;
            using Result = Expected<U, E>;

            if(HasValue())
                return Result(INTERNAL::InPlaceInv{}, [&](){return std::invoke(std::forward<Fn>(f), std::move(m_value));});

            return Result(Unexpect, std::move(m_unexpected));
        }

        /// @brief If *this contains an error value, invokes f with the argument Error() and
        ///        returns a TRAP::Expected object that contains its result; otherwise, returns
        ///        a TRAP::Expected object that contains a copy of the contained expected value
        ///        (obtained from operator*).
        /// @param f A suitable function or Callable object whose call signature returns a non-reference type.
        /// @return A TRAP::Expected object containing either the result of f or an expected value,
        ///         as described above.
        template<typename Fn>
        requires std::is_constructible_v<T, T&>
        [[nodiscard]] constexpr auto TransformError(Fn&& f) &
        {
            using G = INTERNAL::ResultXForm<Fn, E&>;
            using Result = Expected<T, G>;

            if(HasValue())
                return Result(std::in_place, m_value);

            return Result(INTERNAL::UnexpectInv{}, [&](){return std::invoke(std::forward<Fn>(f), m_unexpected);});
        }
        /// @brief If *this contains an error value, invokes f with the argument Error() and
        ///        returns a TRAP::Expected object that contains its result; otherwise, returns
        ///        a TRAP::Expected object that contains a copy of the contained expected value
        ///        (obtained from operator*).
        /// @param f A suitable function or Callable object whose call signature returns a non-reference type.
        /// @return A TRAP::Expected object containing either the result of f or an expected value,
        ///         as described above.
        template<typename Fn>
        requires std::is_constructible_v<T, const T&>
        [[nodiscard]] constexpr auto TransformError(Fn&& f) const&
        {
            using G = INTERNAL::ResultXForm<Fn, const E&>;
            using Result = Expected<T, G>;

            if(HasValue())
                return Result(std::in_place, m_value);

            return Result(INTERNAL::UnexpectInv{}, [&](){return std::invoke(std::forward<Fn>(f), m_unexpected);});
        }
        /// @brief If *this contains an error value, invokes f with the argument Error() and
        ///        returns a TRAP::Expected object that contains its result; otherwise, returns
        ///        a TRAP::Expected object that contains a copy of the contained expected value
        ///        (obtained from operator*).
        /// @param f A suitable function or Callable object whose call signature returns a non-reference type.
        /// @return A TRAP::Expected object containing either the result of f or an expected value,
        ///         as described above.
        template<typename Fn>
        requires std::is_constructible_v<T, T>
        [[nodiscard]] constexpr auto TransformError(Fn&& f) &&
        {
            using G = INTERNAL::ResultXForm<Fn, E&&>;
            using Result = Expected<T, G>;

            if(HasValue())
                return Result(std::in_place, std::move(m_value));

            return Result(INTERNAL::UnexpectInv{}, [&](){return std::invoke(std::forward<Fn>(f), std::move(m_unexpected));});
        }
        /// @brief If *this contains an error value, invokes f with the argument Error() and
        ///        returns a TRAP::Expected object that contains its result; otherwise, returns
        ///        a TRAP::Expected object that contains a copy of the contained expected value
        ///        (obtained from operator*).
        /// @param f A suitable function or Callable object whose call signature returns a non-reference type.
        /// @return A TRAP::Expected object containing either the result of f or an expected value,
        ///         as described above.
        template<typename Fn>
        requires std::is_constructible_v<T, const T>
        [[nodiscard]] constexpr auto TransformError(Fn&& f) const&&
        {
            using G = INTERNAL::ResultXForm<Fn, const E&&>;
            using Result = Expected<T, G>;

            if(HasValue())
                return Result(std::in_place, std::move(m_value));

            return Result(INTERNAL::UnexpectInv{}, [&](){return std::invoke(std::forward<Fn>(f), std::move(m_unexpected));});
        }

        /// @brief Performs comparison operations on expected objects.
        ///        Compares two expected objects. The objects compare equal if and only if
        ///        lhs.HasValue() and rhs.HasValue() are equal, and the contained values are equal.
        /// @param lhs Expected object to compare.
        /// @param rhs Expected object to compare.
        /// @return if lhs.HasValue() != rhs.HasValue(), returns false. Otherwise, if lhs.HasValue() is true,
        ///         returns *lhs == *rhs. Otherwise, returns lhs.Error() == rhs.Error().
        template<typename U, typename E2>
        requires (!std::is_void_v<U>)
        [[nodiscard]] friend constexpr bool operator==(const Expected& lhs, const Expected<U, E2>& rhs)
        {
            if(lhs.HasValue())
                return rhs.HasValue() && static_cast<bool>(*lhs == *rhs);

            return !rhs.HasValue() && static_cast<bool>(lhs.Error() == rhs.Error());
        }

        /// @brief Performs comparison operations on expected objects.
        ///        Compares expected object with a value. The objects compare equal if
        ///        and only if rhs contains an expected value, and the contained value
        ///        is equal to rhs.
        /// @param lhs Expected object to compare.
        /// @param rhs Value to compare to the expected value contained in x.
        /// @return Returns lhs.HasValue() && static_cast<bool>(*lhs == rhs).
        template<typename U>
        [[nodiscard]] friend constexpr bool operator==(const Expected& lhs, const U& rhs)
        {
            return lhs.HasValue() && static_cast<bool>(*lhs == rhs);
        }

        /// @brief Performs comparison operations on expected objects.
        ///        Compares expected object with an unexpected value. The objects compare
        ///        equal if and only if lhs contains an unexpected value, and the contained
        ///        value is equal to rhs.Error().
        /// @param lhs Expected object to compare.
        /// @param rhs Value to compare to the unexpected value contained in rhs.
        /// @return Returns !lhs.HasValue() && static_cast<bool>(lhs.Error() == rhs.Error()).
        template<typename E2>
        [[nodiscard]] friend constexpr bool operator==(const Expected& lhs, const Unexpected<E2>& rhs)
        {
            return !lhs.HasValue() && static_cast<bool>(lhs.Error() == rhs.Error());
        }

        /// @brief Overloads the std::swap algorithm for TRAP::Expected. Exchanges the state of lhs
        ///        with that of rhs. Effectively calls lhs.Swap(rhs).
        /// @param lhs Expected objects whose states to swap.
        /// @param rhs Expected objects whose states to swap.
        friend constexpr void swap(Expected& lhs, Expected& rhs) noexcept(noexcept(lhs.Swap(rhs)))
        requires requires {lhs.Swap(rhs);}
        {
            lhs.Swap(rhs);
        }

    private:
        template<typename, typename>
        friend class Expected;

        template<typename V>
        constexpr void AssignValue(V&& v)
        {
            if(m_hasValue)
                m_value = std::forward<V>(v);
            else
            {
                INTERNAL::ReInit(std::addressof(m_value), std::addressof(m_unexpected), std::forward<V>(v));
                m_hasValue = true;
            }
        }

        template<typename V>
        constexpr void AssignUnexpected(V&& v)
        {
            if(m_hasValue)
            {
                INTERNAL::ReInit(std::addressof(m_unexpected), std::addressof(m_value), std::forward<V>(v));
                m_hasValue = false;
            }
            else
                m_unexpected = std::forward<V>(v);
        }

        //Swap two expected objects when only one has a value.
        //Precondition: this->m_hasValue && !rhs.m_hasValue.
        constexpr void SwapValueUnexpected(Expected& rhs) noexcept(std::conjunction_v<std::is_nothrow_move_constructible<E>,
                                                                                      std::is_nothrow_move_constructible<T>>)
        {
            if constexpr(std::is_nothrow_move_constructible_v<E>)
            {
                INTERNAL::Guard<E> guard(rhs.m_unexpected);
                std::construct_at(std::addressof(rhs.m_value), std::move(m_value));
                rhs.m_hasValue = true;
                std::destroy_at(std::addressof(m_value));
                std::construct_at(std::addressof(m_unexpected), guard.Release());
                m_hasValue = false;
            }
            else
            {
                INTERNAL::Guard<T> guard(m_value);
                std::construct_at(std::addressof(m_unexpected), std::move(rhs.m_unexpected));
                m_hasValue = false;
                std::destroy_at(std::addressof(rhs.m_unexpected));
                std::construct_at(std::addressof(rhs.m_value), guard.Release());
                rhs.m_hasValue = true;
            }
        }

        using InPlaceInv = INTERNAL::InPlaceInv;
        using UnexpectInv = INTERNAL::UnexpectInv;

        template<typename Fn>
        explicit constexpr Expected([[maybe_unused]] InPlaceInv _, Fn&& fn)
            : m_value(std::forward<Fn>(fn)()), m_hasValue(true)
        {}

        template<typename Fn>
        explicit constexpr Expected([[maybe_unused]] UnexpectInv _, Fn&& fn)
            : m_unexpected(std::forward<Fn>(fn)()), m_hasValue(false)
        {}

        union
        {
            T m_value;
            E m_unexpected;
        };
        bool m_hasValue;
    };

    //Partial specialization for TRAP::Expected<cv void, E>
    template<typename T, typename E>
    requires std::is_void_v<T>
    class Expected<T, E>
    {
        static_assert(INTERNAL::CanBeUnexpected<E>);

        template<typename U, typename Err, typename Unex = Unexpected<E>>
        static constexpr bool ConstructFromExpected = std::disjunction_v<std::is_constructible<Unex, Expected<U, Err>&>,
                                                                         std::is_constructible<Unex, Expected<U, Err>>,
                                                                         std::is_constructible<Unex, const Expected<U, Err>&>,
                                                                         std::is_constructible<Unex, const Expected<U, Err>>>;

        template<typename U>
        static constexpr bool SameValue = std::same_as<typename U::value_type, T>;

        template<typename U>
        static constexpr bool SameError = std::same_as<typename U::error_type, E>;

    public:
        using value_type = T;
        using error_type = E;
        using unexpected_type = Unexpected<E>;

        template<typename U>
        using rebind = Expected<U, error_type>;

        constexpr Expected() noexcept
            : m_void(), m_hasValue(true)
        {}

        constexpr Expected(const Expected& other) = default;

        constexpr Expected(const Expected& other) noexcept(std::is_nothrow_copy_constructible_v<E>)
        requires std::is_copy_constructible_v<E> && (!std::is_trivially_copy_constructible_v<E>)
            : m_void(), m_hasValue(other.m_hasValue)
        {
            if(!m_hasValue)
                std::construct_at(std::addressof(m_unexpected), other.m_unexpected);
        }

        constexpr Expected(Expected&& other) = default;

        constexpr Expected(Expected&& other) noexcept(std::is_nothrow_move_constructible_v<E>)
        requires std::is_move_constructible_v<E> && (!std::is_trivially_move_constructible_v<E>)
            : m_void(), m_hasValue(other.m_hasValue)
        {
            if(!m_hasValue)
                std::construct_at(std::addressof(m_unexpected), std::move(other).m_unexpected);
        }

        template<typename U, typename G>
        requires std::is_void_v<U> && std::is_constructible_v<E, const G&> &&
                 (!ConstructFromExpected<U, G>)
        constexpr explicit(!std::is_convertible_v<const G&, E>) Expected(const Expected<U, G>& other) noexcept(std::is_nothrow_constructible_v<E, const G&>)
            : m_void(), m_hasValue(other.m_hasValue)
        {
            if(!m_hasValue)
                std::construct_at(std::addressof(m_unexpected), other.m_unexpected);
        }

        template<typename U, typename G>
        requires std::is_void_v<U> && std::is_constructible_v<E, G> && (!ConstructFromExpected<U, G>)
        constexpr explicit(!std::is_convertible_v<G, E>) Expected(Expected<U, G>&& other) noexcept(std::is_nothrow_constructible_v<E, G>)
            : m_void(), m_hasValue(other.m_hasValue)
        {
            if(!m_hasValue)
                std::construct_at(std::addressof(m_unexpected), std::move(other).m_unexpected);
        }

        template<typename G = E>
        requires std::is_constructible_v<E, const G&>
        constexpr explicit(!std::is_convertible_v<const G&, E>) Expected(const Unexpected<G>& u) noexcept(std::is_nothrow_constructible_v<E, const G&>)
            : m_unexpected(u.Error()), m_hasValue(false)
        {}

        template<typename G = E>
        requires std::is_constructible_v<E, G>
        constexpr explicit(!std::is_convertible_v<G, E>) Expected(Unexpected<G>&& u) noexcept(std::is_nothrow_constructible_v<E, G>)
            : m_unexpected(std::move(u).Error()), m_hasValue(false)
        {}

        constexpr explicit Expected([[maybe_unused]] std::in_place_t _) noexcept
            : Expected()
        {}

        template<typename... Args>
        requires std::is_constructible_v<E, Args...>
        constexpr explicit Expected([[maybe_unused]] Unexpect_t _, Args&&... args) noexcept(std::is_nothrow_constructible_v<E, Args...>)
            : m_unexpected(std::forward<Args>(args)...), m_hasValue(false)
        {}

        template<typename U, typename... Args>
        requires std::is_constructible_v<E, std::initializer_list<U>&, Args...>
        constexpr explicit Expected([[maybe_unused]] Unexpect_t _, std::initializer_list<U> il, Args&&... args) noexcept(std::is_nothrow_constructible_v<E, std::initializer_list<U>&, Args...>)
            : m_unexpected(il, std::forward<Args>(args)...), m_hasValue(false)
        {}

        constexpr ~Expected() = default;

        constexpr ~Expected()
        requires (!std::is_trivially_destructible_v<E>)
        {
            if(!m_hasValue)
                std::destroy_at(std::addressof(m_unexpected));
        }

        consteval Expected& operator=(const Expected& other) = delete;

        constexpr Expected& operator=(const Expected& other) noexcept(std::conjunction_v<std::is_nothrow_copy_constructible<E>,
                                                                                         std::is_nothrow_copy_assignable<E>>)
        requires std::is_copy_constructible_v<E> && std::is_copy_assignable_v<E>
        {
            if(other.m_hasValeu)
                Emplace();
            else
                AssignUnexpected(other.m_unexpected);

            return *this;
        }

        constexpr Expected& operator=(Expected&& other) noexcept(std::conjunction_v<std::is_nothrow_move_constructible<E>,
                                                                                    std::is_nothrow_move_assignable<E>>)
        requires std::is_move_constructible_v<E> && std::is_move_assignable_v<E>
        {
            if(other.m_hasValue)
                Emplace();
            else
                AssignUnexpected(std::move(other.m_unexpected));

            return *this;
        }

        template<typename G>
        requires std::is_constructible_v<E, const G&> && std::is_assignable_v<E&, const G&>
        constexpr Expected& operator=(const Unexpected<G>& e)
        {
            AssignUnexpected(e.Error());
            return *this;
        }

        template<typename G>
        requires std::is_constructible_v<E, G> && std::is_assignable_v<E&, G>
        constexpr Expected& operator=(Unexpected<G>&& e)
        {
            AssignUnexpected(std::move(e.Error()));
            return *this;
        }

        constexpr void Emplace() noexcept
        {
            if(!m_hasValue)
            {
                std::destroy_at(std::addressof(m_unexpected));
                m_hasValue = true;
            }
        }

        constexpr void Swap(Expected& other) noexcept(std::conjunction_v<std::is_nothrow_swappable<E&>,
                                                                         std::is_nothrow_move_constructible<E>>)
        requires std::is_swappable_v<E> && std::is_move_constructible_v<E>
        {
            if(m_hasValue)
            {
                if(!other.m_hasValue)
                {
                    std::construct_at(std::addressof(m_unexpected), std::move(other.m_unexpected));
                    std::destroy_at(std::addressof(other.m_unexpected));
                    m_hasValue = false;
                    other.m_hasValue = true;
                }
            }
            else
            {
                if(other.m_hasValue)
                {
                    std::construct_at(std::addressof(other.m_unexpected), std::move(m_unexpected));
                    std::destroy_at(std::addressof(m_unexpected));
                    m_hasValue = true;
                    other.m_hasValue = false;
                }
                else
                {
                    using std::swap;
                    swap(m_unexpected, other.m_unexpected);
                }
            }
        }

        [[nodiscard]] constexpr explicit operator bool() const noexcept
        {
            return m_hasValue;
        }

        [[nodiscard]] constexpr bool HasValue() const noexcept
        {
            return m_hasValue;
        }

        constexpr void operator*() const noexcept
        {
            TRAP_ASSERT(m_hasValue);
        }

        constexpr void Value() const&
        {
            if(m_hasValue) [[likely]]
                return;

            INTERNAL::ThrowOrAbort(BadExpectedAccess<E>(m_unexpected));
        }

        constexpr void Value() &&
        {
            if(m_hasValue) [[likely]]
                return;

            INTERNAL::ThrowOrAbort(BadExpectedAccess<E>(std::move(m_unexpected)));
        }

        template<typename F>
        requires std::invocable<F>
        constexpr void ValueOrElse(F&& f) const &
        {
            static_assert(std::same_as<T, std::invoke_result_t<F>>,
                          "F must return type void!");

            if(m_hasValue)
                return;

            std::invoke(std::forward<F>(f));
        }
        template<typename F>
        requires std::invocable<F>
        constexpr void ValueOrElse(F&& f) &&
        {
            static_assert(std::same_as<T, std::invoke_result_t<F>>,
                          "F must return type void!");

            if(m_hasValue)
                return;

            std::invoke(std::forward<F>(f));
        }

        [[nodiscard]] constexpr const E& Error() const& noexcept
        {
            TRAP_ASSERT(!m_hasValue);
            return m_unexpected;
        }
        [[nodiscard]] constexpr E& Error() & noexcept
        {
            TRAP_ASSERT(!m_hasValue);
            return m_unexpected;
        }
        [[nodiscard]] constexpr const E&& Error() const && noexcept
        {
            TRAP_ASSERT(!m_hasValue);
            return std::move(m_unexpected);
        }
        [[nodiscard]] constexpr E&& Error() && noexcept
        {
            TRAP_ASSERT(!m_hasValue);
            return std::move(m_unexpected);
        }

        template<typename G = E>
        [[nodiscard]] constexpr E ErrorOr(G&& e) const&
        {
            static_assert(std::is_copy_constructible_v<E>);
            static_assert(std::is_convertible_v<G, E>);

            if(m_hasValue)
                return std::forward<G>(e);

            return m_unexpected;
        }
        template<typename G = E>
        [[nodiscard]] constexpr E ErrorOr(G&& e) &&
        {
            static_assert(std::is_copy_constructible_v<E>);
            static_assert(std::is_convertible_v<G, E>);

            if(m_hasValue)
                return std::forward<G>(e);

            return std::move(m_unexpected);
        }

        template<typename Fn>
        requires std::is_constructible_v<E, E&>
        [[nodiscard]] constexpr auto AndThen(Fn&& f) &
        {
            using U = INTERNAL::Result0<Fn>;
            static_assert(INTERNAL::IsExpected<U>);
            static_assert(std::same_as<typename U::error_type, E>);

            if(HasValue())
                return std::invoke(std::forward<Fn>(f));

            return U(Unexpect, m_unexpected);
        }
        template<typename Fn>
        requires std::is_constructible_v<E, const E&>
        [[nodiscard]] constexpr auto AndThen(Fn&& f) const&
        {
            using U = INTERNAL::Result0<Fn>;
            static_assert(INTERNAL::IsExpected<U>);
            static_assert(std::same_as<typename U::error_type, E>);

            if(HasValue())
                return std::invoke(std::forward<Fn>(f));

            return U(Unexpect, m_unexpected);
        }
        template<typename Fn>
        requires std::is_constructible_v<E, E>
        [[nodiscard]] constexpr auto AndThen(Fn&& f) &&
        {
            using U = INTERNAL::Result0<Fn>;
            static_assert(INTERNAL::IsExpected<U>);
            static_assert(std::same_as<typename U::error_type, E>);

            if(HasValue())
                return std::invoke(std::forward<Fn>(f));

            return U(Unexpect, std::move(m_unexpected));
        }
        template<typename Fn>
        requires std::is_constructible_v<E, const E>
        [[nodiscard]] constexpr auto AndThen(Fn&& f) const&&
        {
            using U = INTERNAL::Result0<Fn>;
            static_assert(INTERNAL::IsExpected<U>);
            static_assert(std::same_as<typename U::error_type, E>);

            if(HasValue())
                return std::invoke(std::forward<Fn>(f));

            return U(Unexpect, std::move(m_unexpected));
        }

        template<typename Fn>
        [[nodiscard]] constexpr auto OrElse(Fn&& f) &
        {
            using G = INTERNAL::Result<Fn, E&>;
            static_assert(INTERNAL::IsExpected<G>);
            static_assert(std::same_as<typename G::value_type, T>);

            if(HasValue())
                return G();

            return std::invoke(std::forward<Fn>(f), m_unexpected);
        }
        template<typename Fn>
        [[nodiscard]] constexpr auto OrElse(Fn&& f) const&
        {
            using G = INTERNAL::Result<Fn, const E&>;
            static_assert(INTERNAL::IsExpected<G>);
            static_assert(std::same_as<typename G::value_type, T>);

            if(HasValue())
                return G();

            return std::invoke(std::forward<Fn>(f), m_unexpected);
        }
        template<typename Fn>
        [[nodiscard]] constexpr auto OrElse(Fn&& f) &&
        {
            using G = INTERNAL::Result<Fn, E&&>;
            static_assert(INTERNAL::IsExpected<G>);
            static_assert(std::same_as<typename G::value_type, T>);

            if(HasValue())
                return G();

            return std::invoke(std::forward<Fn>(f), std::move(m_unexpected));
        }
        template<typename Fn>
        [[nodiscard]] constexpr auto OrElse(Fn&& f) const&&
        {
            using G = INTERNAL::Result<Fn, const E&&>;
            static_assert(INTERNAL::IsExpected<G>);
            static_assert(std::same_as<typename G::value_type, T>);

            if(HasValue())
                return G();

            return std::invoke(std::forward<Fn>(f), std::move(m_unexpected));
        }

        template<typename Fn>
        requires std::is_constructible_v<E, E&>
        [[nodiscard]] constexpr auto Transform(Fn&& f) &
        {
            using U = INTERNAL::Result0XForm<Fn>;
            using Result = Expected<U, E>;

            if(HasValue())
                return Result(INTERNAL::InPlaceInv{}, std::forward<Fn>(f));

            return Result(Unexpect, m_unexpected);
        }
        template<typename Fn>
        requires std::is_constructible_v<E, const E&>
        [[nodiscard]] constexpr auto Transform(Fn&& f) const&
        {
            using U = INTERNAL::Result0XForm<Fn>;
            using Result = Expected<U, E>;

            if(HasValue())
                return Result(INTERNAL::InPlaceInv{}, std::forward<Fn>(f));

            return Result(Unexpect, m_unexpected);
        }
        template<typename Fn>
        requires std::is_constructible_v<E, E>
        [[nodiscard]] constexpr auto Transform(Fn&& f) &&
        {
            using U = INTERNAL::Result0XForm<Fn>;
            using Result = Expected<U, E>;

            if(HasValue())
                return Result(INTERNAL::InPlaceInv{}, std::forward<Fn>(f));

            return Result(Unexpect, std::move(m_unexpected));
        }
        template<typename Fn>
        requires std::is_constructible_v<E, const E>
        [[nodiscard]] constexpr auto Transform(Fn&& f) const&&
        {
            using U = INTERNAL::Result0XForm<Fn>;
            using Result = Expected<U, E>;

            if(HasValue())
                return Result(INTERNAL::InPlaceInv{}, std::forward<Fn>(f));

            return Result(Unexpect, std::move(m_unexpected));
        }

        template<typename Fn>
        [[nodiscard]] constexpr auto TransformError(Fn&& f) &
        {
            using G = INTERNAL::ResultXForm<Fn, E&>;
            using Result = Expected<T, G>;

            if(HasValue())
                return Result();

            return Result(INTERNAL::UnexpectInv{}, [&](){return std::invoke(std::forward<Fn>(f), m_unexpected);});
        }
        template<typename Fn>
        [[nodiscard]] constexpr auto TransformError(Fn&& f) const&
        {
            using G = INTERNAL::ResultXForm<Fn, const E&>;
            using Result = Expected<T, G>;

            if(HasValue())
                return Result();

            return Result(INTERNAL::UnexpectInv{}, [&](){return std::invoke(std::forward<Fn>(f), m_unexpected);});
        }
        template<typename Fn>
        [[nodiscard]] constexpr auto TransformError(Fn&& f) &&
        {
            using G = INTERNAL::ResultXForm<Fn, E&&>;
            using Result = Expected<T, G>;

            if(HasValue())
                return Result();

            return Result(INTERNAL::UnexpectInv{}, [&](){return std::invoke(std::forward<Fn>(f), std::move(m_unexpected));});
        }
        template<typename Fn>
        [[nodiscard]] constexpr auto TransformError(Fn&& f) const&&
        {
            using G = INTERNAL::ResultXForm<Fn, const E&&>;
            using Result = Expected<T, G>;

            if(HasValue())
                return Result();

            return Result(INTERNAL::UnexpectInv{}, [&](){return std::invoke(std::forward<Fn>(f), std::move(m_unexpected));});
        }

        template<typename U, typename E2>
        requires std::is_void_v<U>
        [[nodiscard]] friend constexpr bool operator==(const Expected& lhs, const Expected<U, E2>& rhs)
        {
            if(lhs.HasValue())
                return rhs.HasValue();

            return !rhs.HasValue() && static_cast<bool>(lhs.Error() == rhs.Error());
        }

        template<typename E2>
        [[nodiscard]] friend constexpr bool operator==(const Expected& lhs, const Unexpected<E2>& rhs)
        {
            return !lhs.HasValue() && static_cast<bool>(lhs.Error() == rhs.Error());
        }

        [[nodiscard]] friend constexpr void swap(Expected& lhs, Expected& rhs) noexcept(noexcept(lhs.Swap(rhs)))
        requires requires {lhs.Swap(rhs);}
        {
            lhs.Swap(rhs);
        }

    private:
        template<typename, typename>
        friend class Expected;

        template<typename V>
        constexpr void AssignUnexpected(V&& v)
        {
            if(m_hasValue)
            {
                std::construct_at(std::addressof(m_unexpected), std::forward<V>(v));
                m_hasValue = false;
            }
            else
                m_unexpected = std::forward<V>(v);
        }

        using InPlaceInv = INTERNAL::InPlaceInv;
        using UnexpectInv = INTERNAL::UnexpectInv;

        template<typename Fn>
        explicit constexpr Expected([[maybe_unused]] InPlaceInv _, Fn&& fn)
            : m_void(), m_hasValue(true)
        {
            std::forward<Fn>(fn)();
        }

        template<typename Fn>
        explicit constexpr Expected([[maybe_unused]] UnexpectInv, Fn&& fn)
            : m_unexpected(std::forward<Fn>(fn)()), m_hasValue(false)
        {}

        union
        {
            struct{} m_void;
            E m_unexpected;
        };
        bool m_hasValue;
    };
}

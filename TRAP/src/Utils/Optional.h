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

    /// @brief Defines a type of object to be thrown by TRAP::Optional::Value() when accessing
    ///        an optional object that does not contain a value.
    struct BadOptionalAccess final : std::exception
    {
        /// @brief Constructs a new BadOptionalAccess object with an implementation-defined null-terminated byte
        ///        string which is accessible through what().
        ///
        ///        Default constructor.
        BadOptionalAccess() noexcept = default;

        /// @brief Default destructor.
        ~BadOptionalAccess() override = default;

        /// @brief Constructs a new BadOptionalAccess object with an implementation-defined null-terminated byte
        ///        string which is accessible through what().
        ///
        ///        Copy constructor. If *this and other both have dynamic type TRAP::BadOptionalAccess then
        ///        std::strcmp(what(), other.what()) == 0.
        /// @param other Another exception object to copy.
        constexpr BadOptionalAccess(const BadOptionalAccess& other) noexcept = default;

        /// @brief Constructs a new BadOptionalAccess object with an implementation-defined null-terminated byte
        ///        string which is accessible through what().
        ///
        ///        Move constructor. If *this and other both have dynamic type TRAP::BadOptionalAccess then
        ///        std::strcmp(what(), other.what()) == 0.
        /// @param other Another exception object to move.
        constexpr BadOptionalAccess(BadOptionalAccess&& other) noexcept = default;

        /// @brief Assigns the contents with those of other. If *this and other both have dynamic type
        ///        TRAP::BadOptionalAccess then std::strcmp(what(), other.what()) == 0 after assignment.
        /// @param other Another exception object to assign with.
        /// @return *this.
        BadOptionalAccess& operator=(const BadOptionalAccess& other) noexcept = default;

        /// @brief Assigns the contents with those of other. If *this and other both have dynamic type
        ///        TRAP::BadOptionalAccess then std::strcmp(what(), other.what()) == 0 after assignment.
        /// @param other Another exception object to assign with.
        /// @return *this.
        BadOptionalAccess& operator=(BadOptionalAccess&& other) noexcept = default;

        /// @brief Returns the explanatory string.
        /// @return Pointer to a null-terminated string with explanatory information.
        ///         The string is suitable for conversion and display as a std::wstring.
        ///         The pointer is guaranteed to be valid at least until the exception object from
        ///         which it is obtained is destroyed, or until a non-const member function
        ///         (e.g. copy assignment operator) on the exception object is called.
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

    /// @brief TRAP::NullOptT is an empty class type used to indicate optional type with
    ///        uninitialized state. In particular, TRAP::Optional has a constructor
    ///        with NullOptT as a single argument, which creates an optional that does ot
    ///        contain a value.
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

    /// @brief TRAP::NullOpt is a constant of type TRAP::NullOptT that is used to indicate
    ///        optional type with uninitialized state.
    inline constexpr NullOptT NullOpt{0};
    inline constexpr InPlaceT InPlace;

    /// @brief The class template TRAP::Optional manages an optional contained value, i.e. a value that may or may not
    ///        be present.
    ///
    ///        A common use case for optional is the return value of a function that may fail. As opposed to other
    ///        approaches, such as std::pair<T, bool>, optional handles expensive-to-construct objects well and is more
    ///        readable, as the intent is expressed explicitly.
    ///
    ///        Any instance of Optional<T> at any given point in time either contains a value or does not contain a value.
    ///
    ///        If an Optional<T> contains a value, the value is guaranteed to be allocated as part of the optional
    ///        object footprint, i.e. no dynamic memory allocation ever takes place. Thus, an optional object models
    ///        an object, not a pointer, even though operator*() and operator->() are defined.
    ///
    ///        When an object of type Optional<T> is contextually converted to bool, the conversion returns true
    ///        if the object contains a value and false if it does not contain a value.
    ///
    ///        The optional object contains a value in the following conditions:
    ///            - The object is initialized with/assigned from a value of type T or another optional that
    ///              contains a value.
    ///        The optional object does not contain a value in the following conditions:
    ///            - The object is default-initialized.
    ///            - The object is initialized with/assigned from a value of type TRAP::NullOptT or an optional object
    ///              that does not contain a value.
    ///            - The member function Reset() is called.
    ///
    ///        This implementation supports references with rebinding on assignment.
    ///
    ///        There are no optional functions, arrays, or cv void; a program is ill-formed if it instantiates
    ///        an optional with such type. In addition, a program is ill-formed if it instantiates an optional
    ///        with the (possibly cv-qualified) tag types TRAP::NullOptT or TRAP::InPlaceT.
    /// @tparam T The type of the value to manage initialization state for. The type must meet the requiremnts of
    ///           Destructible (in particular, array types are not allowed).
    template<typename T>
    class Optional final
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

        /// @brief Constructs a new optional object.
        ///
        ///        Constructs an object that does not contain a value.
        constexpr Optional() noexcept
            : Optional(NullOpt)
        {
        }

        /// @brief Constructs a new optional object.
        ///
        ///        Constructs an object that does not contain a value.
        constexpr Optional(NullOptT) noexcept
        {
        }

        /// @brief Constructs a new optional object.
        ///
        ///        Copy constructor: If other contains a value, initializes the contained value as if direct-initializing
        ///        (but not direct-list-initializing) an object of type T with the expression *other. If other does not
        ///        contain a value, constructs an object that does not contain a value.
        /// @param other Another optional object whose contained value is copied.
        constexpr Optional(const Optional& other)
        requires std::is_trivially_copy_constructible_v<T> = default;

        /// @brief Constructs a new optional object.
        ///
        ///        Copy constructor: If other contains a value, initializes the contained value as if direct-initializing
        ///        (but not direct-list-initializing) an object of type T with the expression *other. If other does not
        ///        contain a value, constructs an object that does not contain a value.
        /// @param other Another optional object whose contained value is copied.
        constexpr Optional(const Optional& other)
        requires (std::is_copy_constructible_v<T> && !std::is_trivially_copy_constructible_v<T>)
            : m_active(other.m_active)
        {
            ConstructFromOptional(other);
        }

        /// @brief Constructs a new optional object.
        ///
        ///        Move constructor: If other contains a value, initializes the contained value as if direct-initializing
        ///        (but not direct-list-initializing) an object of type T with the expression std::move(*other) and does
        ///        not make other empty: a moved-from TRAP::Optional still contains a value, but the value itself is
        ///        moved from. If other does not contain a value, constructs an object that does not contain a value.
        /// @param other Another optional object whose contained value is copied.
        constexpr Optional(Optional&& other)
        requires std::is_trivially_move_constructible_v<T> = default;

        /// @brief Constructs a new optional object.
        ///
        ///        Move constructor: If other contains a value, initializes the contained value as if direct-initializing
        ///        (but not direct-list-initializing) an object of type T with the expression std::move(*other) and does
        ///        not make other empty: a moved-from TRAP::Optional still contains a value, but the value itself is
        ///        moved from. If other does not contain a value, constructs an object that does not contain a value.
        /// @param other Another optional object whose contained value is copied.
        constexpr Optional(Optional&& other) noexcept(std::is_nothrow_move_constructible_v<T>)
        requires (std::is_move_constructible_v<T> && !std::is_trivially_move_constructible_v<T>)
            : m_active(other.m_active)
        {
            ConstructFromOptional(std::forward<Optional>(other));
        }

        /// @brief Constructs a new optional object.
        ///
        ///        Constructs an optional object that contains a value, initialized as if direct-initializing
        ///        (but not direct-list-initializing) an object of type T with the expression std::forward<U>(value).
        /// @param u Value with which to initialize the contained value.
        template<typename U = T>
        constexpr explicit(!std::is_convertible_v<U, T>) Optional(U&& u)
        requires (std::is_constructible_v<T, U> &&
                  !std::is_same_v<std::remove_cvref_t<U>, InPlaceT> &&
                  !std::is_same_v<std::remove_cvref_t<U>, Optional>)
            : m_active(true)
        {
            ConstructFrom(std::forward<U>(u));
        }

        /// @brief Constructs a new optional object.
        ///
        ///        Constructs an optional object that contains a value, initialized as if direct-initializing
        ///        (but not direct-list-initializing) an object of type T from the arguments std::forward<Args>(args)....
        /// @param args Arguments with which to initialize the contained value.
        template<typename... Args>
        constexpr explicit Optional(InPlaceT, Args&&... args)
        requires std::is_constructible_v<T, Args...>
            : m_active(true)
        {
            ConstructFrom(std::forward<Args>(args)...);
        }

        /// @brief Constructs a new optional object.
        ///
        ///        Constructs an optional object that contains a value, initialized as if direct-initializing
        ///        (but not direct-list-initializing) an object of type T from the arguments il, std::forward<Args>(args)....
        /// @param il Initializer list with which to initialize the contained value.
        /// @param args Arguments with which to initialize the contained value.
        template<typename U, typename... Args>
        constexpr explicit Optional(InPlaceT, std::initializer_list<U> il, Args&&... args)
        requires std::is_constructible_v<T, std::initializer_list<U>&, Args...>
            : m_active(true)
        {
            ConstructFrom(il, std::forward<Args>(args)...);
        }

        /// @brief Constructs a new optional object.
        ///
        ///        Converting copy constructor: If other does not contain a value, constructs an optional object that
        ///        does not contain a value. Otherwise, constructs an optional object that contains a value, initialized
        ///        as if direct-initializing (but not direct-list-initializing) an object of type T with the
        ///        expression *other.
        /// @param other Another optional object whose contained value is copied.
        template<typename U = T>
        constexpr explicit(!std::is_convertible_v<const U&, T>) Optional(const Optional<U>& other)
        requires INTERNAL::OptionalConstructorRequirement<T, const U&, U>
            : m_active(other.HasValue())
        {
            ConstructFromOptional(other);
        }

        /// @brief Constructs a new optional object.
        ///
        ///        Converting move constructor: If other does not contain a value, constructs an optional object that does
        ///        not contain a value. Otherwise, constructs an optional object that contains a value, initialized
        ///        as if direct-initializing (but not direct-list-initializing) an object of type T with the expression
        ///        std::move(*other).
        /// @param other Another optional object whose contained value is copied.
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

        /// @brief If the object contains a value and the type T is not trivially destructible, destroys the contained
        ///        value by calling its destructor, as if by Value().T::~T().
        ///        Otherwise, does nothing.
        constexpr ~Optional()
        requires std::is_trivially_destructible_v<T> = default;

        /// @brief If the object contains a value and the type T is not trivially destructible, destroys the contained
        ///        value by calling its destructor, as if by Value().T::~T().
        ///        Otherwise, does nothing.
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

        /// @brief Replaces contents of *this with the contents of other.
        ///
        ///        If *this contains a value before the call, the contained value is destroyed by calling its destructor
        ///        as if by Value().T::~T(). *this does not contain a value after this call.
        /// @return *this.
        constexpr Optional& operator=(NullOptT) noexcept
        {
            if(!m_active)
                return *this;

            ResetNoCheck();
            return *this;
        }

        /// @brief Replaces contents of *this with the contents of other.
        ///
        ///        Assigns the state of other.
        ///        - If both *this and other do not contain a value, the function has no effect.
        ///        - If *this contains a value, but other does not, then the contained value is destroyed by calling
        ///          its destructor.
        ///        - If other contains a value, then depending on whether *this contains a value, the contained value
        ///          is either direct-initialized or assigned from *other.
        /// @param other Another optional object whose contained value to assign.
        /// @return *this.
        constexpr Optional& operator=(const Optional& other)
        requires INTERNAL::IsTriviallyCopyAssignableAndConstructible<T>
        = default;

        /// @brief Replaces contents of *this with the contents of other.
        ///
        ///        Assigns the state of other.
        ///        - If both *this and other do not contain a value, the function has no effect.
        ///        - If *this contains a value, but other does not, then the contained value is destroyed by calling
        ///          its destructor.
        ///        - If other contains a value, then depending on whether *this contains a value, the contained value
        ///          is either direct-initialized or assigned from *other.
        /// @param other Another optional object whose contained value to assign.
        /// @return *this.
        constexpr Optional& operator=(const Optional& other)
        requires (INTERNAL::IsCopyAssignableAndConstructible<T> && !INTERNAL::IsTriviallyCopyAssignableAndConstructible<T>)
        {
            return AssignFromOptional(other);
        }

        /// @brief Replaces contents of *this with the contents of other.
        ///
        ///        Assigns the state of other.
        ///        - If both *this and other do not contain a value, the function has no effect.
        ///        - If *this contains a value, but other does not, then the contained value is destroyed by calling
        ///          its destructor.
        ///        - If other contians a value, then depending on whether *this contains a value, the contained value
        ///          is either direct-initialized or assigned from std::move(*other). Note that a moved-from optional
        ///          still contains a value.
        /// @param other Another optional object whose contained value to assign.
        /// @return *this.
        constexpr Optional& operator=(Optional&& other)
        requires INTERNAL::IsTriviallyMoveAssignableAndConstructible<T>
        = default;

        /// @brief Replaces contents of *this with the contents of other.
        ///
        ///        Assigns the state of other.
        ///        - If both *this and other do not contain a value, the function has no effect.
        ///        - If *this contains a value, but other does not, then the contained value is destroyed by calling
        ///          its destructor.
        ///        - If other contians a value, then depending on whether *this contains a value, the contained value
        ///          is either direct-initialized or assigned from std::move(*other). Note that a moved-from optional
        ///          still contains a value.
        /// @param other Another optional object whose contained value to assign.
        /// @return *this.
        constexpr Optional& operator=(Optional&& other)
        requires (INTERNAL::IsMoveAssignableAndConstructible<T> && !INTERNAL::IsTriviallyMoveAssignableAndConstructible<T>)
        {
            return AssignFromOptional(std::forward<Optional>(other));
        }

        /// @brief Replaces contents of *this with the contents of other.
        ///
        ///        Perfect-forwarded assignment: depending on whether *this contains a value before the call, the
        ///        contained value is either direct-initialized from std::forward<U>(value) or assigned from
        ///        std::forward<U>(value).
        /// @param u Value to assign to the contained value.
        /// @return *this.
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

        /// @brief Replaces contents of *this with the contents of other.
        ///
        ///        Assigns the state of other.
        ///        - If both *this and other do not contain a value, the function has no effect.
        ///        - If *this contains a value, but other does not, then the contained value is destroyed by calling
        ///          its destructor. *this does not contain a value after the call.
        ///        - If other contains a value, then depending on whether *this contains value, the contained
        ///          value is either direct-initialized or assigned from *other.
        /// @param u Another optional object whose contained value to assign.
        /// @return *this.
        template<typename U>
        constexpr Optional& operator=(const Optional<U>& u)
        requires INTERNAL::OptionalAssignmentRequirement<T, const U&, U>
        {
            return AssignFromOptional(u);
        }

        /// @brief Replaces contents of *this with the contents of other.
        ///
        ///        Assigns the state of other.
        ///        - If both *this and other do not contain a value, the function has no effect.
        ///        - If *this contains a value, but other does not, then the contained value is destroyed by calling
        ///          its destructor. *this does not contain a value after the call.
        ///        - If other contains a value, then depending on whether *this contains value, the contained
        ///          value is either direct-initialized or assigned from std::move(*other). Note that a moved-from
        ///          optional still contains a value.
        /// @param u Another optional object whose contained value to assign.
        /// @return *this.
        template<typename U>
        constexpr Optional& operator=(Optional<U>&& u)
        requires INTERNAL::OptionalAssignmentRequirement<T, U, U>
        {
            return AssignFromOptional(std::forward<Optional<U>>(u));
        }

        //-------------------------------------------------------------------------------------------------------------------//
        //Observers----------------------------------------------------------------------------------------------------------//
        //-------------------------------------------------------------------------------------------------------------------//

        /// @brief Accesses the contained value.
        ///        The behavior is undefined if *this does not contain a value.
        /// @return Pointer to the contained value.
        [[nodiscard]] constexpr const T* operator->() const noexcept
        {
            return Ptr();
        }

        /// @brief Accesses the contained value.
        ///        The behavior is undefined if *this does not contain a value.
        /// @return Pointer to the contained value.
        [[nodiscard]] constexpr T* operator->() noexcept
        {
            return Ptr();
        }

        /// @brief Accesses the contained value.
        ///        The behavior is undefined if *this does not contain a value.
        /// @return Reference to the contained value.
        [[nodiscard]] constexpr const T& operator*() const& noexcept
        {
            return m_value;
        }

        /// @brief Accesses the contained value.
        ///        The behavior is undefined if *this does not contain a value.
        /// @return Reference to the contained value.
        [[nodiscard]] constexpr T& operator*() & noexcept
        {
            return m_value;
        }

        /// @brief Accesses the contained value.
        ///        The behavior is undefined if *this does not contain a value.
        /// @return Reference to the contained value.
        [[nodiscard]] constexpr const T&& operator*() const&& noexcept
        {
            return std::move(m_value);
        }

        /// @brief Accesses the contained value.
        ///        The behavior is undefined if *this does not contain a value.
        /// @return Reference to the contained value.
        [[nodiscard]] constexpr T&& operator*() && noexcept
        {
            return std::move(m_value);
        }

        /// @brief Checks whether *this contains a value.
        /// @return true if *this contains a value, false if *this does not contain a value.
        [[nodiscard]] constexpr explicit operator bool() const noexcept
        {
            return m_active;
        }

        /// @brief Checks whether *this contains a value.
        /// @return true if *this contains a value, false if *this does not contain a value.
        [[nodiscard]] constexpr bool HasValue() const noexcept
        {
            return m_active;
        }

        /// @brief If *this contains a value, returns a reference to the contained value.
        ///        Otherwise, throws a TRAP::BadOptionalAccess exception.
        /// @return Reference to the contained value.
        /// @note The dereference operator operator*() does not check if this optional contains a value,
        ///       which may be more efficient than Value().
        [[nodiscard]] constexpr T& Value() &
        {
            if (*this)
                return **this;

            INTERNAL::DoThrow<BadOptionalAccess>();
        }

        /// @brief If *this contains a value, returns a reference to the contained value.
        ///        Otherwise, throws a TRAP::BadOptionalAccess exception.
        /// @return Reference to the contained value.
        /// @note The dereference operator operator*() does not check if this optional contains a value,
        ///       which may be more efficient than Value().
        [[nodiscard]] constexpr const T& Value() const&
        {
            if(*this)
                return **this;

            INTERNAL::DoThrow<BadOptionalAccess>();
        }

        /// @brief If *this contains a value, returns a reference to the contained value.
        ///        Otherwise, throws a TRAP::BadOptionalAccess exception.
        /// @return Reference to the contained value.
        /// @note The dereference operator operator*() does not check if this optional contains a value,
        ///       which may be more efficient than Value().
        [[nodiscard]] constexpr T&& Value() &&
        {
            if(*this)
                return std::move(m_value);

            INTERNAL::DoThrow<BadOptionalAccess>();
        }

        /// @brief If *this contains a value, returns a reference to the contained value.
        ///        Otherwise, throws a TRAP::BadOptionalAccess exception.
        /// @return Reference to the contained value.
        /// @note The dereference operator operator*() does not check if this optional contains a value,
        ///       which may be more efficient than Value().
        [[nodiscard]] constexpr const T&& Value() const&&
        {
            if(*this)
                return std::move(m_value);

            INTERNAL::DoThrow<BadOptionalAccess>();
        }

        /// @brief Returns the contained value if *this has a value, otherwise returns u.
        /// @param u The value to use in case *this is empty.
        /// @return The current value if *this has a value, or u otherwise.
        template<typename U>
        [[nodiscard]] constexpr T ValueOr(U&& u) const &
        requires (std::is_copy_constructible_v<T> && std::is_convertible_v<U&&, T>)
        {
            return bool(*this) ? **this : static_cast<T>(std::forward<U>(u));
        }

        /// @brief Returns the contained value if *this has a value, otherwise returns u.
        /// @param u The value to use in case *this is empty.
        /// @return The current value if *this has a value, or u otherwise.
        template<typename U>
        [[nodiscard]] constexpr T ValueOr(U&& u) &&
        requires (std::is_move_constructible_v<T> && std::is_convertible_v<U&&, T>)
        {
            return bool(*this) ? std::move(m_value) : static_cast<T>(std::forward<U>(u));
        }

        //-------------------------------------------------------------------------------------------------------------------//
        //Modifiers----------------------------------------------------------------------------------------------------------//
        //-------------------------------------------------------------------------------------------------------------------//

        /// @brief Construct the contained value in-place. If *this already contains a value before the call,
        ///        the contained value is destroyed by calling its destructor.
        ///
        ///        Initializes the contained value by direct-initializing (but not direct-list-initializing) with
        ///        std::forward<Args>(args)... as parameters.
        /// @param args The arguments to pass to the constructor.
        /// @return Reference to the new contained value.
        template<typename... Args>
        requires std::is_constructible_v<T, Args...>
        constexpr void Emplace(Args&&... args)
        {
            Reset();
            ConstructFrom(std::forward<Args>(args)...);
        }

        /// @brief Construct the contained value in-place. If *this already contains a value before the call,
        ///        the contained value is destroyed by calling its destructor.
        ///
        ///        Initializes the contained value by direct-initializing (but not direct-list-initializing) with
        ///        std::forward<Args>(args)... as parameters.
        /// @param args The arguments to pass to the constructor.
        /// @return Reference to the new contained value.
        template<typename U, typename... Args>
        requires std::is_constructible_v<T, std::initializer_list<U>&, Args...>
        constexpr void Emplace(std::initializer_list<U> il, Args&&... args)
        {
            Reset();
            ConstructFrom(il, std::forward<Args>(args)...);
        }

        /// @brief Swaps the contents with those of other.
        ///
        ///        - If neither *this nor other contain a value, the function has no effect.
        ///        - If only one of *this and other contains a value (let's call this object in
        ///          and the other un), the contained value of un is direct-initialized from std::move(*in),
        ///          followed by destruction of the contained value of in as if by in->T::~T().
        ///          After this call, in does not contain a value; un contains a value.
        ///        - If both *this and other contain values, the contained values are exchanged.
        /// @param other The optional object to exchange the contents with
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

        /// @brief If *this contains a value, destroy that value as if by Value().T::~T(). Otherwise,
        ///        there are no effects. *this does not contain a value after this call.
        constexpr void Reset() noexcept
        {
            if(!m_active)
                return;

            ResetNoCheck();
        }

        /// @brief Take the current value out of this optional, leaving it empty.
        /// @return Returns the current value.
        [[nodiscard]] constexpr Optional Take()
        {
            Optional ret = std::move(*this);
            Reset();
            return ret;
        }

        /// @brief If *this contains a value, invokes f with the contained value as an argument,
        ///        and returns an TRAP::Optional that contains the result of that invocation;
        ///        otherwise, returns an empty TRAP::Optional.
        /// @param f A suitable function or Callable object whose call signature returns a non-reference type.
        /// @return An TRAP::Optional containing the result of f or an empty TRAP::Optional, as described above.
        template<typename F, typename Ret = std::remove_cv_t<std::invoke_result_t<F, T&>>>
        constexpr auto Transform(F&& f) &
        requires (!std::is_void_v<Ret>)
        {
            if(m_active)
                return Optional<Ret>(std::invoke(std::forward<F>(f), **this));

            return Optional<Ret>(NullOpt);
        }

        /// @brief If *this contains a value, invokes f with the contained value as an argument,
        ///        and returns an TRAP::Optional that contains the result of that invocation;
        ///        otherwise, returns an empty TRAP::Optional.
        /// @param f A suitable function or Callable object whose call signature returns a non-reference type.
        /// @return An TRAP::Optional containing the result of f or an empty TRAP::Optional, as described above.
        template<typename F, typename Ret = std::remove_cv_t<std::invoke_result_t<F, const T&>>>
        requires (!std::is_void_v<Ret>)
        constexpr auto Transform(F&& f) const&
        {
            if(m_active)
                return Optional<Ret>(std::invoke(std::forward<F>(f), **this));

            return Optional<Ret>(NullOpt);
        }

        /// @brief If *this contains a value, invokes f with the contained value as an argument,
        ///        and returns an TRAP::Optional that contains the result of that invocation;
        ///        otherwise, returns an empty TRAP::Optional.
        /// @param f A suitable function or Callable object whose call signature returns a non-reference type.
        /// @return An TRAP::Optional containing the result of f or an empty TRAP::Optional, as described above.
        template<typename F, typename Ret = std::remove_cv_t<std::invoke_result_t<F, T>>>
        requires (!std::is_void_v<Ret>)
        constexpr auto Transform(F&& f) &&
        {
            if(m_active)
                return Optional<Ret>(std::invoke(std::forward<F>(f), std::move(**this)));

            return Optional<Ret>(NullOpt);
        }

        /// @brief If *this contains a value, invokes f with the contained value as an argument,
        ///        and returns an TRAP::Optional that contains the result of that invocation;
        ///        otherwise, returns an empty TRAP::Optional.
        /// @param f A suitable function or Callable object whose call signature returns a non-reference type.
        /// @return An TRAP::Optional containing the result of f or an empty TRAP::Optional, as described above.
        template<typename F, typename Ret = std::remove_cv_t<std::invoke_result_t<F, const T>>>
        requires (!std::is_void_v<Ret>)
        constexpr auto Transform(F&& f) const&&
        {
            if(m_active)
                return Optional<Ret>(std::invoke(std::forward<F>(f), std::move(**this)));

            return Optional<Ret>(NullOpt);
        }

        /// @brief If *this contains a value, invokes f with the contained value as an
        ///        argument, and returns the result of that invocation; otherwise, returns
        ///        an empty TRAP::Optional.
        /// @param f A suitable function or Callable object that returns an TRAP::Optional.
        /// @return The result of f or an empty TRAP::Optional, as described above.
        template<typename F>
        constexpr auto AndThen(F&& f) &
        {
            using Ret = std::invoke_result_t<F, T&>;
            static_assert(IsOptional<Ret>::value, "F must return an Optional");

            if(m_active)
                return std::invoke(std::forward<F>(f), **this);

            return std::remove_cvref_t<std::invoke_result_t<F, T&>>(NullOpt);
        }

        /// @brief If *this contains a value, invokes f with the contained value as an
        ///        argument, and returns the result of that invocation; otherwise, returns
        ///        an empty TRAP::Optional.
        /// @param f A suitable function or Callable object that returns an TRAP::Optional.
        /// @return The result of f or an empty TRAP::Optional, as described above.
        template<typename F>
        constexpr auto AndThen(F&& f) const&
        {
            using Ret = std::invoke_result_t<F, const T&>;
            static_assert(IsOptional<Ret>::value, "F must return an Optional");

            if(m_active)
                return std::invoke(std::forward<F>(f), **this);

            return std::remove_cvref_t<std::invoke_result_t<F, const T&>>(NullOpt);
        }

        /// @brief If *this contains a value, invokes f with the contained value as an
        ///        argument, and returns the result of that invocation; otherwise, returns
        ///        an empty TRAP::Optional.
        /// @param f A suitable function or Callable object that returns an TRAP::Optional.
        /// @return The result of f or an empty TRAP::Optional, as described above.
        template<typename F>
        constexpr auto AndThen(F&& f) &&
        {
            using Ret = std::invoke_result_t<F, T>;
            static_assert(IsOptional<Ret>::value, "F must return an Optional");

            if(m_active)
                return std::invoke(std::forward<F>(f), std::move(**this));

            return std::remove_cvref_t<std::invoke_result_t<F, T>>(NullOpt);
        }

        /// @brief If *this contains a value, invokes f with the contained value as an
        ///        argument, and returns the result of that invocation; otherwise, returns
        ///        an empty TRAP::Optional.
        /// @param f A suitable function or Callable object that returns an TRAP::Optional.
        /// @return The result of f or an empty TRAP::Optional, as described above.
        template<typename F>
        constexpr auto AndThen(F&& f) const&&
        {
            using Ret = std::invoke_result_t<F, const T>;
            static_assert(IsOptional<Ret>::value, "F must return an Optional");

            if(m_active)
                return std::invoke(std::forward<F>(f), std::move(**this));

            return std::remove_cvref_t<std::invoke_result_t<F, const T>>(NullOpt);
        }

        /// @brief Returns *this if it contains a value. Otherwise, returns the result of f.
        /// @param f A function or Callable object that returns an TRAP::Optional<T>.
        /// @return *this or the result of f, as described above.
        template<typename F>
        constexpr Optional<T> OrElse(F&& f) const&
        requires (std::copy_constructible<T> && std::invocable<F>)
        {
            using Ret = std::remove_cvref_t<std::invoke_result_t<F>>;
            static_assert(IsOptional<Ret>::value, "F must return an Optional");

            return *this ? *this : std::forward<F>(f)();

        }

        /// @brief Returns *this if it contains a value. Otherwise, returns the result of f.
        /// @param f A function or Callable object that returns an TRAP::Optional<T>.
        /// @return *this or the result of f, as described above.
        template<typename F>
        constexpr Optional<T> OrElse(F&& f) &&
        requires (std::move_constructible<T> && std::invocable<F>)
        {
            using Ret = std::remove_cvref_t<std::invoke_result_t<F>>;
            static_assert(IsOptional<Ret>::value, "F must return an Optional");

            return *this ? std::move(*this) : std::forward<F>(f)();
        }

        /// @brief Returns the current value if it contains a value.
        ///        Otherwise, returns the result of f.
        /// @param f A function or Callable object that returns an T.
        /// @return The current value or the result of f, as described above.
        template<typename F>
        [[nodiscard]] constexpr T ValueOrElse(F&& f) const&
        requires (std::copy_constructible<T> && std::invocable<F>)
        {
            return m_active ? m_value : std::forward<F>(f)();
        }

        /// @brief Returns the current value if it contains a value.
        ///        Otherwise, returns the result of f.
        /// @param f A function or Callable object that returns an T.
        /// @return The current value or the result of f, as described above.
        template<typename F>
        [[nodiscard]] constexpr T ValueOrElse(F&& f) &&
        requires (std::move_constructible<T> && std::invocable<F>)
        {
            return m_active ? std::move(**this) : std::forward<F>(f)();
        }

        /// @brief Returns the result of f if optional contains a value.
        ///        Otherwise, returns the result of u.
        /// @param f A function or Callable object that returns an T.
        /// @return The current value or the result of u, as described above.
        template<typename F, typename U>
        [[nodiscard]] constexpr auto TransformOrElse(F&& f, U&& u) const&
        {
            return m_active ? std::invoke(std::forward<F>(f), **this) : std::forward<U>(u)();
        }

        /// @brief Returns the result of f if optional contains a value.
        ///        Otherwise, returns the result of u.
        /// @param f A function or Callable object that returns an T.
        /// @return The current value or the result of u, as described above.
        template<typename F, typename U>
        [[nodiscard]] constexpr auto TransformOrElse(F&& f, U&& u) &&
        {
            return m_active ? std::invoke(std::forward<F>(f), std::move(**this)) : std::forward<U>(u)();
        }

        /// @brief Returns the result of f if optional contains a value.
        ///        Otherwise, returns the value u.
        /// @param f A function or Callable object that returns an T.
        /// @return The current value or u, as described above.
        template<typename F, typename U>
        [[nodiscard]] constexpr U TransformOr(F&& f, U&& u) const&
        {
            return m_active ? std::invoke(std::forward<F>(f), **this) : std::forward<U>(u);
        }

        /// @brief Returns the result of f if optional contains a value.
        ///        Otherwise, returns the value u.
        /// @param f A function or Callable object that returns an T.
        /// @return The current value or u, as described above.
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
    class Optional<T&> final
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

    /// @brief Overloads the std::swap algorithm for TRAP::Optional. Exchanges the state of x
    ///        with that of y. Effectively calls lhs.Swap(rhs).
    /// @param x Optional object whose states to swap.
    /// @param y Optional object whose states to swap.
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

    /// @brief Creates an optional object from v.
    /// @param v The value to construct optional object with.
    /// @return The constructed optional object.
    template<typename T = INTERNAL::IAmSecret, typename U, typename Ret = std::conditional_t<std::is_same_v<T, INTERNAL::IAmSecret>, std::decay_t<U>, T>>
    [[nodiscard]] constexpr auto MakeOptional(U&& v)
    {
        return Optional<std::remove_cvref_t<Ret>>(std::forward<U>(v));
    }

    /// @brief Creates an optional object constructed in-place from args....
    /// @param args Arguments to be passed to the constructor of T.
    /// @return The constructed optional object.
    template<typename T, typename... Args>
    [[nodiscard]] constexpr auto MakeOptional(Args&&... args)
    {
        return Optional<std::remove_cvref_t<T>>{InPlace, std::forward<Args>(args)...};
    }

    /// @brief Creates an optional object constructed in-place from il and args....
    /// @param il Arguments to be passed to the constructor of T.
    /// @param args Arguments to be passed to the constructor of T.
    /// @return The constructed optional object.
    template<typename T, typename U, typename... Args>
    [[nodiscard]] constexpr auto MakeOptional(std::initializer_list<U> il, Args&&... args)
    {
        return Optional<std::remove_cvref_t<T>>{InPlace, il, std::forward<Args>(args)...};
    }
}

namespace std
{
    /// @brief The template specialization of std::hash for the TRAP::Optional class allows
    ///        users to obtain hashes of the values contained in optional objects.
    /// @note For an optional that does not contain a value, the hash is unspecified.
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

#ifndef TRAP_SAFE_H
#define TRAP_SAFE_H

#include <mutex>
#include <shared_mutex>

//Tracy - Profiler
#include <tracy/Tracy.hpp>

#include "ContentionFreeSharedMutex.h"

namespace TRAP::Utils
{
    namespace INTERNAL
    {
        using DefaultMutex = SharedLockableBase(ContentionFreeSharedMutex<>);
        // using DefaultMutex = SharedLockableBase(std::shared_mutex);
        template<typename MutexType>
        using DefaultReadOnlyLock = std::shared_lock<MutexType>;
        template<typename MutexType>
        using DefaultReadWriteLock = std::unique_lock<MutexType>;

        template<typename Type>
        struct MutableIfNotReference
        {
            mutable Type Get;
        };

        template<typename Type>
        struct MutableIfNotReference<Type&>
        {
            Type& Get;
        };

        template<typename T>
        concept SupportsAtomic = std::integral<T> || std::same_as<T, bool>;
    }

    enum class AccessMode
    {
        ReadOnly,
        ReadWrite
    };

    template<typename LockType>
    struct AccessTraits
    {
        static constexpr bool IsReadOnly = false;
    };

    template<typename MutexType>
    struct AccessTraits<std::lock_guard<MutexType>>
    {
        static constexpr bool IsReadOnly = false;
    };

    template<typename... MutexTypes>
    struct AccessTraits<std::scoped_lock<MutexTypes...>>
    {
        static constexpr bool IsReadOnly = false;
    };

    template<typename MutexType>
    struct AccessTraits<std::unique_lock<MutexType>>
    {
        static constexpr bool IsReadOnly = false;
    };

    template<typename MutexType>
    struct AccessTraits<std::shared_lock<MutexType>>
    {
        static constexpr bool IsReadOnly = true;
    };

    //Use this tag to default construct the mutex when constructing a Safe object.
    struct DefaultConstructMutex{};
    static constexpr DefaultConstructMutex DefaultConstructMutexTag;

    /// @brief Wraps a value together with a mutex.
    /// @tparam ValueType The type of the value to protect.
    /// @tparam MutexType The type of the mutex.
    template<typename ValueType, typename MutexType = INTERNAL::DefaultMutex>
    class Safe
    {
        static_assert(!INTERNAL::SupportsAtomic<ValueType>,
                      "ValueType support std::atomic, use it instead of Safe!");

        template<template<typename> class LockType, AccessMode Mode>
        class Access;

        using RemoveRefValueType = typename std::remove_reference_t<ValueType>;
        using RemoveRefMutexType = typename std::remove_reference_t<MutexType>;

        using ConstValueReferenceType = const RemoveRefValueType&;
        using ValueReferenceType = RemoveRefValueType&;
        using MutexReferenceType = RemoveRefMutexType&;

    public:
        //Aliases to ReadAccess and WriteAccess classes for this Safe class.
        template<template<typename> class LockType = INTERNAL::DefaultReadOnlyLock>
        using ReadAccess = Access<LockType, AccessMode::ReadOnly>;
        template<template<typename> class LockType = INTERNAL::DefaultReadWriteLock>
        using WriteAccess = Access<LockType, AccessMode::ReadWrite>;

        /// @brief Construct a Safe object.
        Safe() = default;

        /// @brief Construct a Safe object with default construction of the mutex and
        ///        perfect forwarding of the other arguments to construct the value object.
        /// @param valueArgs Perfect forwarding arguments to construct the value object.
        template<typename... ValueArgs>
        explicit Safe([[maybe_unused]] DefaultConstructMutex _, ValueArgs&&... valueArgs)
            : m_mutex(), m_value(std::forward<ValueArgs>(valueArgs)...)
        {
            ZoneScoped;
        }

        /// @brief Construct a Safe object, forwarding the first argument to construct the
        ///        mutex and the other arguments to construct the value object.
        /// @param mutexArg Perfect forwarding argument to construct the mutex object.
        /// @param valueArgs Perfect forwarding arguments to construct the value object.
        template<typename MutexArg, typename... ValueArgs>
        explicit Safe(MutexArg&& mutexArg, ValueArgs&&... valueArgs)
            : m_mutex{std::forward<MutexArg>(mutexArg)}, m_value(std::forward<ValueArgs>(valueArgs)...)
        {
            ZoneScoped;
        }

        //Delete all coyp/move construction/assignment, as these operations require locking the mutex under th covers.
        //Use Copy(), Assign() and other defined constructors to get the behaviour you need with an explicit syntax.
        consteval Safe(const Safe&) = delete;
        consteval Safe(Safe&&) = delete;
        consteval Safe& operator=(const Safe&) = delete;
        consteval Safe& operator=(Safe&&) = delete;

        template<template<typename> class LockType = INTERNAL::DefaultReadOnlyLock>
        [[nodiscard]] ReadAccess<LockType> ReadLock() const
        {
            ZoneScoped;
            return ReadAccess<LockType>{*this};
        }

        template<template<typename> class LockType = INTERNAL::DefaultReadOnlyLock, typename... LockArgs>
        [[nodiscard]] ReadAccess<LockType> ReadLock(LockArgs&&... lockArgs) const
        {
            ZoneScoped;
            return ReadAccess<LockType>{*this, std::forward<LockArgs>(lockArgs)...};
        }

        template<template<typename> class LockType = INTERNAL::DefaultReadWriteLock>
        [[nodiscard]] WriteAccess<LockType> WriteLock()
        {
            ZoneScoped;
            return WriteAccess<LockType>{*this};
        }

        template<template<typename> class LockType = INTERNAL::DefaultReadWriteLock, typename... LockArgs>
        [[nodiscard]] WriteAccess<LockType> WriteLock(LockArgs&&... lockArgs)
        {
            ZoneScoped;
            return WriteAccess<LockType>{*this, std::forward<LockArgs>(lockArgs)...};
        }

        template<template<typename> class LockType = INTERNAL::DefaultReadOnlyLock, typename... LockArgs>
        [[nodiscard]] RemoveRefValueType Copy(LockArgs&&... lockArgs) const
        {
            ZoneScoped;
            return *ReadLock<LockType>(std::forward<LockArgs>(lockArgs)...);
        }

        template<template<typename> class LockType = INTERNAL::DefaultReadWriteLock, typename... LockArgs>
        void Assign(ConstValueReferenceType value, LockArgs&&... lockArgs)
        {
            ZoneScoped;
            *WriteLock<LockType>(std::forward<LockArgs>(lockArgs)...) = value;
        }

        template<template<typename> class LockType = INTERNAL::DefaultReadWriteLock, typename... LockArgs>
        void Assign(RemoveRefValueType&& value, LockArgs&&... lockArgs)
        {
            ZoneScoped;
            *WriteLock<LockType>(std::forward<LockArgs>(lockArgs)...) = std::move(value);
        }

        /// @brief Unsafe const accessor to the value.
        /// @return Const reference to the value object.
        /// @note If you use this function, you exit the realm of safety!
        [[nodiscard]] constexpr ConstValueReferenceType GetUnsafe() const noexcept
        {
            return m_value;
        }

        /// @brief Unsafe accessor to the value.
        /// @return Reference to the value object.
        /// @note If you use this function, you exit the realm of safety!
        [[nodiscard]] constexpr ValueReferenceType GetUnsafe() noexcept
        {
            return m_value;
        }

        /// @brief Accessor to the mutex.
        /// @return Reference to the mutex.
        /// @note Use this to lock multiple Safe objects at the same time by using std::scoped_lock.
        [[nodiscard]] MutexReferenceType GetMutex() const noexcept
        {
            ZoneScoped;
            return m_mutex.Get;
        }

    private:
        /// @brief Manages a mutex and gives pointer-like access to a value object.
        /// @tparam LockType The type of the Lock object that manages the mutex, example: std::lock_guard.
        /// @tparam Mode Determines the access mode of the Access object.
        ///         Can be either AccessMode::ReadOnly or AccessMode::ReadWrite.
        template<template<typename> class LockType, AccessMode Mode>
        class Access
        {
            //Make sure AccessMode is ReadOnly if a read-only lock is used.
            static_assert(!(AccessTraits<LockType<RemoveRefMutexType>>::IsReadOnly && Mode == AccessMode::ReadWrite),
                          "Cannot have ReadWrite access mode with ReadOnly lock. Check the value of AccessTraits<LockType>::IsReadOnly if it exists.");

            using ConstIfReadOnlyValueType = typename std::conditional_t<Mode == AccessMode::ReadOnly, const RemoveRefValueType, RemoveRefValueType>;

        public:
            using ConstPointerType = const ConstIfReadOnlyValueType*;
            using PointerType = ConstIfReadOnlyValueType*;
            using ConstReferenceType = const ConstIfReadOnlyValueType&;
            using ReferenceType = ConstIfReadOnlyValueType&;

            /// @brief Construct an Access object from a possibly const reference to the value
            ///        object and any additional argument needed to construct the Lock object.
            /// @param value Reference to the value.
            /// @param otherLockArgs Arguments needed to construct the Lock object.
            template<typename... OtherLockArgs>
            explicit Access(ReferenceType value, MutexType& mutex, OtherLockArgs&&... otherLockArgs)
                : Lock(mutex, std::forward<OtherLockArgs>(otherLockArgs)...), m_value(value)
            {
                ZoneScoped;
                LockMark(mutex);
            }

            /// @brief Construct a read-only Access object from a const Safe object and any
            ///        additional argument needed to construct the Lock object.
            ///
            ///        If needed, you can provide additional arguments to construct the Lock object (such as std::adopt_lock).
            ///        The mutex from the Lockable object is already passed to the Lock object's constructor though, you must not provide it.
            /// @param safe The const Safe object to give protected access to.
            /// @param otherLockArgs Other arguments needed to construct the Lock object.
            template<typename... OtherLockArgs>
            explicit Access(const Safe& safe, OtherLockArgs&&... otherLockArgs)
                : Access(safe.m_value, safe.m_mutex.Get, std::forward<OtherLockArgs>(otherLockArgs)...)
            {
            }

            /// @brief Construct a read-write Access object from a Safe object and any
            ///        additional argument needed to construct the Lock object.
            ///
            ///        If needed, you can provide additional arguments to construct the Lock object (such as std::adopt_lock).
            ///        The mutex from the Safe object is already passed to the Lock object's constructor though, you must not provide it.
            /// @param safe The Safe object to give protected access to.
            /// @param otherLockArgs Other arguments needed to construct the Lock object.
            template<typename... OtherLockArgs>
            explicit Access(Safe& safe, OtherLockArgs&&... otherLockArgs)
                : Access(safe.m_value, safe.m_mutex.Get, std::forward<OtherLockArgs>(otherLockArgs)...)
            {
            }

            /// @brief Construct an Access object from another one.
            /// @param otherAccess The Acces sobject to construct from.
            /// @param otherLockArgs Other arguments needed to construct the lock object.
            /// @note OtherLockType must implement release() like std::unique_lock does.
            template<template<typename> class OtherLockType, AccessMode OtherMode, typename... OtherLockArgs>
            explicit Access(Access<OtherLockType, OtherMode>& otherAccess, OtherLockArgs&&... otherLockArgs)
                : Access(*otherAccess, *otherAccess.lock().release(), std::adopt_lock, std::forward<OtherLockArgs>(otherLockArgs)...)
            {
                static_assert(OtherMode == AccessMode::ReadWrite || OtherMode == Mode,
                              "Cannot construct a ReadWrite Access object from a ReadOnly one!");
            }

            /// @brief Const accessor to the value.
            /// @return Const pointer to the protected value.
            [[nodiscard]] constexpr ConstPointerType operator->() const noexcept
            {
                return &m_value;
            }

            /// @brief Accessor to the value.
            /// @return Pointer to the protected value.
            [[nodiscard]] constexpr PointerType operator->() noexcept
            {
                return &m_value;
            }

            /// @brief Const accessor to the value.
            /// @return Const reference to the protected value.
            [[nodiscard]] constexpr ConstReferenceType operator*() const noexcept
            {
                return m_value;
            }

            /// @brief Accessor to the value.
            /// @return Reference to the protected value.
            [[nodiscard]] constexpr ReferenceType operator*() noexcept
            {
                return m_value;
            }

            /// @brief The lock that manages the mutex
            mutable LockType<RemoveRefMutexType> Lock;

        private:
            /// @brief The protected value.
            ReferenceType m_value;
        };

#ifdef TRACY_ENABLE
        #define TracyAnyLockableCustom( type, varname ) varname { [] () -> const tracy::SourceLocationData* { static constexpr tracy::SourceLocationData srcloc { nullptr, #type " " #varname, TracyFile, TracyLine, 0 }; return &srcloc; }() }
        INTERNAL::MutableIfNotReference<MutexType> TracyAnyLockableCustom(MutexType, m_mutex);
        #undef TracyAnyLockableCustom
#else
        INTERNAL::MutableIfNotReference<MutexType> m_mutex{};
#endif

        //The value to protect.
        ValueType m_value;
    };

    /// @brief Type alias for read-only access.
    /// @tparam SafeType The type of Safe object to give read-only access to.
    /// @tparam LockType The type of lock.
    template<typename SafeType, template<typename> class LockType = INTERNAL::DefaultReadOnlyLock>
    using ReadAccess = typename SafeType::template ReadAccess<LockType>;

    /// @brief Type alias for read-write access.
    /// @tparam SafeType The type of Safe object to give read-write access to.
    /// @tparam LockType The type of lock.
    template<typename SafeType, template<typename> class LockType = INTERNAL::DefaultReadWriteLock>
    using WriteAccess = typename SafeType::template WriteAccess<LockType>;
}

#endif /*TRAP_SAFE_H*/

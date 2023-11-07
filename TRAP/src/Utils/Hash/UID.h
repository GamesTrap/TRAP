#ifndef TRAP_UID_H
#define TRAP_UID_H

#include <functional>

#include "Core/Types.h"

namespace TRAP::Utils
{
    /// @brief Unique identifier, basically a randomly generated 64-bit number
    class UID
    {
    public:
        /// @brief Constructor. Generated a new unique identifier.
        UID();
        /// @brief Constructor.
        constexpr explicit UID(u64 uid) noexcept;

        /// @brief Destructor.
        constexpr ~UID() = default;

        /// @brief Copy constructor.
        constexpr UID(const UID& other) noexcept = default;
        /// @brief Move constructor.
        constexpr UID(UID&& other) noexcept = default;

        /// @brief Copy assignment operator.
        constexpr UID& operator=(const UID& other) noexcept = default;
        /// @brief Move assignment operator.
        constexpr UID& operator=(UID&& other) noexcept = default;

        /// @brief Retrieve the unique identifier as a u64.
        /// @return Unique identifier as u64.
        [[nodiscard]] constexpr operator u64() const noexcept;

    private:
        u64 m_uid;
    };
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Utils::UID::UID(u64 uid) noexcept
    : m_uid(uid)
{
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Utils::UID::operator u64() const noexcept
{
    return m_uid;
}

//-------------------------------------------------------------------------------------------------------------------//

namespace std
{
    template<>
    struct hash<TRAP::Utils::UID>
    {
        [[nodiscard]] constexpr usize operator()(const TRAP::Utils::UID& uid) const noexcept
        {
            return uid;
        }
    };
}

#endif /*TRAP_UID_H*/
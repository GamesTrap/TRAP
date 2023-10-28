#ifndef TRAP_UID_H
#define TRAP_UID_H

#include <functional>

#include "Core/Types.h"

namespace TRAP::Utils
{
    //Unique identifier, basically a randomly generated 64-bit number
    class UID
    {
    public:
        UID();
        constexpr explicit UID(u64 uid) noexcept;

        constexpr ~UID() = default;

        constexpr UID(const UID& other) noexcept = default;
        constexpr UID(UID&& other) noexcept = default;

        constexpr UID& operator=(const UID& other) noexcept = default;
        constexpr UID& operator=(UID&& other) noexcept = default;

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
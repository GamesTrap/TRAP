#ifndef TRAP_UID_H
#define TRAP_UID_H

#include <cstdint>
#include <functional>

namespace TRAP::Utils
{
    //Unique identifier, basically a randomly generated 64-bit number
    class UID
    {
    public:
        UID();
        constexpr explicit UID(uint64_t uid) noexcept;

        constexpr ~UID() = default;

        constexpr UID(const UID& other) noexcept = default;
        constexpr UID(UID&& other) noexcept = default;

        constexpr UID& operator=(const UID& other) noexcept = default;
        constexpr UID& operator=(UID&& other) noexcept = default;

        [[nodiscard]] constexpr operator uint64_t() const noexcept;

    private:
        uint64_t m_uid;
    };
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Utils::UID::UID(uint64_t uid) noexcept
    : m_uid(uid)
{
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Utils::UID::operator uint64_t() const noexcept
{
    return m_uid;
}

//-------------------------------------------------------------------------------------------------------------------//

namespace std
{
    template<>
    struct hash<TRAP::Utils::UID>
    {
        [[nodiscard]] constexpr std::size_t operator()(const TRAP::Utils::UID& uid) const noexcept
        {
            return uid;
        }
    };
}

#endif /*TRAP_UID_H*/
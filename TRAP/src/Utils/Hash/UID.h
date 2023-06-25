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
        explicit UID(uint64_t uid) noexcept;

        constexpr ~UID() = default;

        UID(const UID& other) noexcept;
        UID(UID&& other) noexcept;

        UID& operator=(const UID& other) noexcept = default;
        UID& operator=(UID&& other) noexcept = default;

        operator uint64_t() const noexcept;

    private:
        uint64_t m_uid;
    };
}

//-------------------------------------------------------------------------------------------------------------------//

namespace std
{
    template<>
    struct hash<TRAP::Utils::UID>
    {
        std::size_t operator()(const TRAP::Utils::UID& uid) const noexcept
        {
            return uid;
        }
    };
}

#endif /*TRAP_UID_H*/
#include "TRAPPCH.h"
#include "UID.h"

#include "Utils/Random/Random.h"

TRAP::Utils::UID::UID()
    : m_uid(TRAP::Utils::RandomThreadLocal::Get())
{
    ZoneScoped;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Utils::UID::UID(uint64_t uid)
    : m_uid(uid)
{
    ZoneScoped;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Utils::UID::UID(const UID& other) noexcept
    : m_uid(other.m_uid)
{
    ZoneScoped;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Utils::UID::UID(UID&& other) noexcept
    : m_uid(other.m_uid)
{
    ZoneScoped;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Utils::UID::operator uint64_t()
{
    ZoneScoped;

    return m_uid;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Utils::UID::operator uint64_t() const
{
    ZoneScoped;

    return m_uid;
}

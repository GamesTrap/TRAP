#include "TRAPPCH.h"
#include "UID.h"

#include "Utils/Random/Random.h"

TRAP::Utils::UID::UID()
    : m_uid(TRAP::Utils::RandomThreadLocal::Get())
{
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Utils::UID::UID(uint64_t uid)
    : m_uid(uid)
{
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Utils::UID::UID(const UID& other) noexcept
    : m_uid(other.m_uid)
{
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Utils::UID::UID(UID&& other) noexcept
    : m_uid(other.m_uid)
{
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Utils::UID::operator uint64_t()
{
    return m_uid;
}

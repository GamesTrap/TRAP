#include "TRAPPCH.h"
#include "UID.h"

#include "Utils/Random/Random.h"

TRAP::Utils::UID::UID()
    : m_uid(TRAP::Utils::RandomThreadLocal::Get())
{
    ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Utils::UID::UID(uint64_t uid) noexcept
    : m_uid(uid)
{
    ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Utils::UID::UID(const UID& other) noexcept
    : m_uid(other.m_uid)
{
    ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Utils::UID::UID(UID&& other) noexcept
    : m_uid(other.m_uid)
{
    ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Utils::UID::operator uint64_t() const noexcept
{
    ZoneNamedC(__tracy, tracy::Color::Violet, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

    return m_uid;
}

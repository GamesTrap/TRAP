#include "TRAPPCH.h"
#include "UID.h"

#include "Utils/Random/Random.h"

TRAP::Utils::UID::UID()
    : m_uid(TRAP::Utils::RandomThreadLocal::Get())
{
    ZoneNamedC(__tracy, tracy::Color::Violet, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None);
}

#include "TRAPPCH.h"
#include "Entity.h"

#include "Scene/Components.h"
#include "Utils/Hash/UID.h"

[[nodiscard]] const TRAP::Utils::UID& TRAP::Entity::GetUID()
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return GetComponent<UIDComponent>().UID;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] const std::string& TRAP::Entity::GetName()
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return GetComponent<TagComponent>().Tag;
}

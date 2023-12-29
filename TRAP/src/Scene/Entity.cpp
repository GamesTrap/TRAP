#include "TRAPPCH.h"
#include "Entity.h"

#include "Scene/Components.h"
#include "Utils/Hash/UID.h"

[[nodiscard]] const TRAP::Utils::UID& TRAP::Entity::GetUID() const
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, (GetTRAPProfileSystems() & ProfileSystems::Scene) != ProfileSystems::None &&
	                                             (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	return GetComponent<UIDComponent>().UID;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] const std::string& TRAP::Entity::GetName() const
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, (GetTRAPProfileSystems() & ProfileSystems::Scene) != ProfileSystems::None &&
	                                             (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	return GetComponent<TagComponent>().Tag;
}

#include "TRAPPCH.h"
#include "Entity.h"

#include "Scene/Components.h"
#include "Utils/Hash/UID.h"

TRAP::Entity::Entity(const entt::entity handle, Scene* const scene) noexcept
	: m_entityHandle(handle), m_scene(scene)
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Utils::UID TRAP::Entity::GetUID()
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

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Entity::operator bool() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_entityHandle != entt::null;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Entity::operator uint32_t() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return static_cast<uint32_t>(m_entityHandle);
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Entity::operator entt::entity() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_entityHandle;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Entity::operator==(const Entity other) const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_entityHandle == other.m_entityHandle && m_scene == other.m_scene;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Entity::operator!=(const Entity other) const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return !operator==(other);
}
#include "TRAPPCH.h"
#include "Entity.h"

#include "Scene/Components.h"
#include "Utils/Hash/UID.h"

TRAP::Entity::Entity(const entt::entity handle, Scene* const scene)
	: m_entityHandle(handle), m_scene(scene)
{
	ZoneScoped;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Utils::UID TRAP::Entity::GetUID()
{
	ZoneScoped;

	return GetComponent<UIDComponent>().UID;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::string& TRAP::Entity::GetName()
{
	ZoneScoped;

	return GetComponent<TagComponent>().Tag;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Entity::operator bool() const
{
	ZoneScoped;

	return m_entityHandle != entt::null;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Entity::operator uint32_t() const
{
	ZoneScoped;

	return static_cast<uint32_t>(m_entityHandle);
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Entity::operator entt::entity() const
{
	ZoneScoped;

	return m_entityHandle;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Entity::operator==(const Entity other) const
{
	ZoneScoped;

	return m_entityHandle == other.m_entityHandle && m_scene == other.m_scene;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Entity::operator!=(const Entity other) const
{
	ZoneScoped;

	return !operator==(other);
}
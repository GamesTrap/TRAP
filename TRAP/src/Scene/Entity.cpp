#include "TRAPPCH.h"
#include "Entity.h"

TRAP::Entity::Entity(const entt::entity handle, Scene* scene)
	: m_entityHandle(handle), m_scene(scene)
{}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Entity::operator bool() const
{
	return m_entityHandle != entt::null;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Entity::operator uint32_t() const
{
	return static_cast<uint32_t>(m_entityHandle);
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Entity::operator entt::entity() const
{
	return m_entityHandle;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Entity::operator==(const Entity other) const
{
	return m_entityHandle == other.m_entityHandle && m_scene == other.m_scene;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Entity::operator!=(const Entity other) const
{
	return !operator==(other);
}
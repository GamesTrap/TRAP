#include "TRAPPCH.h"
#include "Entity.h"

#include "Scene/Components.h"
#include "Utils/Hash/UID.h"

TRAP::Entity::Entity(const entt::entity handle, Scene* const scene)
	: m_entityHandle(handle), m_scene(scene)
{
	ZoneScopedC(tracy::Color::Turquoise);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Utils::UID TRAP::Entity::GetUID()
{
	ZoneScopedC(tracy::Color::Turquoise);

	return GetComponent<UIDComponent>().UID;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::string& TRAP::Entity::GetName()
{
	ZoneScopedC(tracy::Color::Turquoise);

	return GetComponent<TagComponent>().Tag;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Entity::operator bool() const
{
	ZoneScopedC(tracy::Color::Turquoise);

	return m_entityHandle != entt::null;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Entity::operator uint32_t() const
{
	ZoneScopedC(tracy::Color::Turquoise);

	return static_cast<uint32_t>(m_entityHandle);
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Entity::operator entt::entity() const
{
	ZoneScopedC(tracy::Color::Turquoise);

	return m_entityHandle;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Entity::operator==(const Entity other) const
{
	ZoneScopedC(tracy::Color::Turquoise);

	return m_entityHandle == other.m_entityHandle && m_scene == other.m_scene;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Entity::operator!=(const Entity other) const
{
	ZoneScopedC(tracy::Color::Turquoise);

	return !operator==(other);
}
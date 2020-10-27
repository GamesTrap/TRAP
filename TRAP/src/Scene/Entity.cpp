#include "TRAPPCH.h"
#include "Entity.h"

TRAP::Entity::Entity(const entt::entity handle, Scene* scene)
	: m_entityHandle(handle), m_scene(scene)
{}
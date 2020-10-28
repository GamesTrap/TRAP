#ifndef _TRAP_ENTITY_H_
#define _TRAP_ENTITY_H_

#include "Scene.h"

#pragma warning(push, 0)
#include "entt.h"
#pragma warning(pop)

namespace TRAP
{
	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity handle, Scene* scene);
		Entity(const Entity&) = default;
		Entity(Entity&&) = default;
		Entity& operator=(const Entity&) = default;
		Entity& operator=(Entity&&) = default;

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			TRAP_ASSERT(!HasComponent<T>(), "Entity already has component!");
			
			return m_scene->m_registry.emplace<T>(m_entityHandle, std::forward<Args>(args)...);
		}

		template<typename T>
		T& GetComponent()
		{
			TRAP_ASSERT(HasComponent<T>(), "Entity does not have component!");
			
			return m_scene->m_registry.get<T>(m_entityHandle);
		}
		
		template<typename T>
		bool HasComponent()
		{
			return m_scene->m_registry.has<T>(m_entityHandle);
		}

		template<typename T>
		void RemoveComponent()
		{
			TRAP_ASSERT(HasComponent<T>(), "Entity does not have component!");

			m_scene->m_registry.remove<T>(m_entityHandle);
		}

		operator bool() const
		{
			return m_entityHandle != entt::null;
		}
		operator uint32_t() const
		{
			return static_cast<uint32_t>(m_entityHandle);
		}

		bool operator==(const Entity other) const
		{
			return m_entityHandle == other.m_entityHandle && m_scene == other.m_scene;
		}
		bool operator!=(const Entity other) const
		{
			return !operator==(other);
		}
		
	private:
		entt::entity m_entityHandle{ entt::null };
		Scene* m_scene = nullptr;
	};
}

#endif /*_TRAP_ENTITY_H_*/
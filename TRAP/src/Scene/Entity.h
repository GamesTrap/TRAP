#ifndef _TRAP_ENTITY_H_
#define _TRAP_ENTITY_H_

#include "Core/PlatformDetection.h"
#include "Core/Base.h"
#include "TRAP_Assert.h"
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
			T& component = m_scene->m_registry.emplace<T>(m_entityHandle, std::forward<Args>(args)...);
			m_scene->OnComponentAdded<T>(*this, component);
			return component;
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

		operator bool() const;
		operator entt::entity() const;
		operator uint32_t() const;

		bool operator==(Entity other) const;
		bool operator!=(Entity other) const;
		
	private:
		entt::entity m_entityHandle{ entt::null };
		Scene* m_scene = nullptr;
	};
}

#endif /*_TRAP_ENTITY_H_*/
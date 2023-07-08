#ifndef TRAP_ENTITY_H
#define TRAP_ENTITY_H

#include "Core/PlatformDetection.h"
#include "Core/Base.h"
#include "TRAP_Assert.h"
#include "Scene.h"

#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif /*_MSC_VER*/
#include <entt.hpp>
#ifdef _MSC_VER
	#pragma warning(pop)
#endif /*_MSC_VER*/

namespace TRAP
{
	namespace Utils
	{
		class UID;
	}

	/// <summary>
	/// Entity class.
	/// </summary>
	class Entity
	{
	public:
		constexpr Entity() noexcept = default;
		/// <summary>
		/// Constructor.
		/// Creates a new Entity.
		/// </summary>
		/// <param name="handle">Handle to entt::entity.</param>
		/// <param name="scene">Scene which the new Entity is associated with.</param>
		constexpr Entity(entt::entity handle, Scene* scene) noexcept;

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			ZoneNamedC(__tracy, tracy::Color::Turquoise, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene);

			TRAP_ASSERT(!HasComponent<T>(), "Entity::AddComponent(): Entity already has component!");
			T& component = m_scene->m_registry.emplace<T>(m_entityHandle, std::forward<Args>(args)...);
			return component;
		}

		template<typename T, typename... Args>
		T& AddOrReplaceComponent(Args&&... args)
		{
			ZoneNamedC(__tracy, tracy::Color::Turquoise, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene);

			T& component = m_scene->m_registry.emplace_or_replace<T>(m_entityHandle, std::forward<Args>(args)...);
			return component;
		}

		template<typename T>
		[[nodiscard]] T& GetComponent()
		{
			ZoneNamedC(__tracy, tracy::Color::Turquoise, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene);

			TRAP_ASSERT(HasComponent<T>(), "Entity::GetComponent(): Entity does not have component!");

			return m_scene->m_registry.get<T>(m_entityHandle);
		}

		template<typename T>
		[[nodiscard]] bool HasComponent()
		{
			ZoneNamedC(__tracy, tracy::Color::Turquoise, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene);

			//Renamed in EnTT v3.7.0 from registry::has<T...> to registry::all_of<T...>
			return m_scene->m_registry.all_of<T>(m_entityHandle);
			// return m_scene->m_registry.has<T>(m_entityHandle);
		}

		template<typename T>
		void RemoveComponent()
		{
			ZoneNamedC(__tracy, tracy::Color::Turquoise, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene);

			TRAP_ASSERT(HasComponent<T>(), "Entity::RemoveComponent(): Entity does not have component!");

			m_scene->m_registry.remove<T>(m_entityHandle);
		}

		[[nodiscard]] Utils::UID GetUID();
		[[nodiscard]] const std::string& GetName();

		constexpr operator bool() const noexcept;
		constexpr operator uint32_t() const noexcept;
		constexpr operator entt::entity() const noexcept;

		constexpr bool operator==(Entity other) const noexcept;
		constexpr bool operator!=(Entity other) const noexcept;

	private:
		entt::entity m_entityHandle{ entt::null };
		Scene* m_scene = nullptr;
	};
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Entity::Entity(const entt::entity handle, Scene* const scene) noexcept
	: m_entityHandle(handle), m_scene(scene)
{
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Entity::operator bool() const noexcept
{
	return m_entityHandle != entt::null;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Entity::operator uint32_t() const noexcept
{
	return static_cast<uint32_t>(m_entityHandle);
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Entity::operator entt::entity() const noexcept
{
	return m_entityHandle;
}

//-------------------------------------------------------------------------------------------------------------------//

inline constexpr bool TRAP::Entity::operator==(Entity other) const noexcept
{
	return m_entityHandle == other.m_entityHandle && m_scene == other.m_scene;
}

//-------------------------------------------------------------------------------------------------------------------//

inline constexpr bool TRAP::Entity::operator!=(Entity other) const noexcept
{
	return !(*this == other);
}

#endif /*TRAP_ENTITY_H*/
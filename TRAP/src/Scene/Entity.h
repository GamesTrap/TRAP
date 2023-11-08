#ifndef TRAP_ENTITY_H
#define TRAP_ENTITY_H

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

	/// @brief Entity class.
	class Entity
	{
	public:
		/// @brief Constructor.
		///        Creates an empty Entity.
		constexpr Entity() noexcept = default;
		/// @brief Constructor.
		/// Creates a new Entity.
		/// @param handle Handle to entt::entity.
		/// @param scene Scene which the new Entity is associated with.
		constexpr Entity(entt::entity handle, Scene* scene) noexcept;

		/// @brief Copy constructor.
		consteval Entity(const Entity&) = default;
		/// @brief Copy assignment operator.
		consteval Entity& operator=(const Entity&) = default;
		/// @brief Move constructor.
		constexpr Entity(Entity&&) noexcept = default;
		/// @brief Move assignment operator.
		constexpr Entity& operator=(Entity&&) noexcept = default;

		/// @brief Destructor.
		constexpr ~Entity() = default;

		/// @brief Add component T to the entity.
		/// @tparam T Type of component to add.
		/// @param args Arguments to forward to the component.
		/// @return N Added component.
		/// @note If entity already has component then the existing component is returned instead.
		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			ZoneNamedC(__tracy, tracy::Color::Turquoise, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene);

			if(HasComponent<T>())
			{
				TRAP_ASSERT(false, "Entity::AddComponent(): Entity already has component!");
				return m_scene->m_registry.get<T>(m_entityHandle);
			}

			return m_scene->m_registry.emplace<T>(m_entityHandle, std::forward<Args>(args)...);
		}

		/// @brief Add a new or replace an existing component.
		/// @tparam T Type of component to add or replace.
		/// @param args Arguments to forward to the component.
		/// @return Added or replaced component.
		template<typename T, typename... Args>
		T& AddOrReplaceComponent(Args&&... args)
		{
			ZoneNamedC(__tracy, tracy::Color::Turquoise, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene);

			T& component = m_scene->m_registry.emplace_or_replace<T>(m_entityHandle, std::forward<Args>(args)...);
			return component;
		}

		/// @brief Retrieve component T from entity.
		/// @tparam T Type of component.
		/// @return Requested component.
		/// @warning Behaviour is undefined if entity doesnt have the requested component.
		template<typename T>
		[[nodiscard]] T& GetComponent() const
		{
			ZoneNamedC(__tracy, tracy::Color::Turquoise, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene);

			TRAP_ASSERT(HasComponent<T>(), "Entity::GetComponent(): Entity does not have component!");

			return m_scene->m_registry.get<T>(m_entityHandle);
		}

		/// @brief Query if entity contains the component T.
		/// @tparam T Type of component.
		/// @return True if entity contains component T, false otherwise.
		template<typename T>
		[[nodiscard]] bool HasComponent() const
		{
			ZoneNamedC(__tracy, tracy::Color::Turquoise, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene);

			return m_scene->m_registry.all_of<T>(m_entityHandle);
		}

		/// @brief Remove component T from entity.
		/// @tparam T Type of component.
		template<typename T>
		void RemoveComponent()
		{
			ZoneNamedC(__tracy, tracy::Color::Turquoise, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene);

			if(!HasComponent<T>())
			{
				TRAP_ASSERT(false, "Entity::RemoveComponent(): Entity does not have component!");
				return;
			}

			m_scene->m_registry.remove<T>(m_entityHandle);
		}

		/// @brief Retrieve the unique identifier of the entity.
		/// @return Unique identifier of the entity.
		[[nodiscard]] const Utils::UID& GetUID() const;
		/// @brief Retrieve the name of the entity.
		/// @return Name of the entity.
		[[nodiscard]] const std::string& GetName() const;

		/// @brief Retrieve whether the entity is valid or null.
		/// @return True if entity is valid, false if the entity is null.
		[[nodiscard]] constexpr operator bool() const noexcept;
		/// @brief Retrieve the underlying entt entity.
		/// @return Underlying entt entity.
		[[nodiscard]] constexpr operator entt::entity() const noexcept;

		[[nodiscard]] constexpr bool operator==(const Entity& other) const noexcept = default;
		[[nodiscard]] constexpr bool operator!=(const Entity& other) const noexcept = default;

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

[[nodiscard]] constexpr TRAP::Entity::operator bool() const noexcept
{
	return m_entityHandle != entt::null;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Entity::operator entt::entity() const noexcept
{
	return m_entityHandle;
}

#endif /*TRAP_ENTITY_H*/
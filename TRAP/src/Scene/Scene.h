#ifndef TRAP_SCENE_H
#define TRAP_SCENE_H

#include "Core/Base.h"

#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif /*_MSC_VER*/
#include <entt.hpp>
#include <box2d/b2_world.h>
#ifdef _MSC_VER
	#pragma warning(pop)
#endif /*_MSC_VER*/

namespace TRAP
{
	namespace Utils
	{
		class TimeStep;
		class UID;
	}
	namespace Graphics
	{
		class EditorCamera;
	}

	class Entity;

	class Scene
	{
	public:
		/// @brief Constructor.
		Scene() noexcept = default;
		/// @brief Destructor.
		~Scene() noexcept = default;

		/// @brief Copy constructor.
		consteval Scene(const Scene&) = delete;
		/// @brief Move constructor.
		Scene(Scene&&) noexcept = default;
		/// @brief Copy assignment operator.
		consteval Scene& operator=(const Scene&) = delete;
		/// @brief Move assignment operator.
		Scene& operator=(Scene&&) noexcept = default;

		/// @brief Create a copy of the given scene.
		/// @param other Scene to copy.
		/// @return Copied scene.
		/// @note Copied scene entities unique identifiers don't match those of the original scene!
		static TRAP::Ref<Scene> Copy(const Scene& other);

		/// @brief Create a new entity with the given name.
		/// @param name Name for the entity.
		/// @return Newly created entity.
		Entity CreateEntity(const std::string& name = std::string());
		/// @brief Destroy the given entity from the scene.
		/// @note This invalidates all Entities that refer to the same underlying instance!
		/// @param entity Entity to destroy.
		void DestroyEntity(const Entity& entity);

		/// @brief Function to call on run time start.
		void OnRuntimeStart();
		/// @brief Function to call on run time stop.
		void OnRuntimeStop();

		/// @brief Function to call on run time for each tick.
		/// @param deltaTime Delta time for fixed tick rate.
		void OnTickRuntime(const TRAP::Utils::TimeStep& deltaTime);
		/// @brief Function to call on run time for each frame.
		/// @param deltaTime Delta time between frames.
		void OnUpdateRuntime(Utils::TimeStep deltaTime);
#ifndef TRAP_HEADLESS_MODE
		/// @brief Function to call in TRAP-Editor for each frame while not in run mode.
		/// @remark @headless This function is not available in headless mode.
		void OnUpdateEditor(Utils::TimeStep deltaTime, const Graphics::EditorCamera& camera);
#endif /*TRAP_HEADLESS_MODE*/
		void OnTick(const TRAP::Utils::TimeStep& deltaTime);
		/// @brief Function to call on viewport resize.
		/// @param width New viewport width.
		/// @param height New viewport height.
		void OnViewportResize(u32 width, u32 height);

		/// @brief Duplicate the given entity.
		///        Create a new entity and copies all components from the given entity.
		/// @param entity Entity to duplicate.
		TRAP::Entity DuplicateEntity(const Entity& entity);

		/// @brief Retrieve the primary camera entity for the scene.
		/// @return Primary camera entity.
		/// @note If no camera was found a null entity is returned instead.
		[[nodiscard]] Entity GetPrimaryCameraEntity();

		/// @brief Retrieve all entities that contain the given components.
		/// @tparam Components Components to retrieve.
		/// @return Found entities with requested components.
		template<typename... Components>
		[[nodiscard]] auto GetAllEntitiesWithComponents()
		{
			return m_registry.view<Components...>();
		}

	private:
		friend class Entity;
		friend class SceneSerializer;
		friend class SceneGraphPanel;

		TRAP::Scope<b2World> m_physicsWorld = nullptr;

		entt::registry m_registry;
	};
}

#endif /*TRAP_SCENE_H*/

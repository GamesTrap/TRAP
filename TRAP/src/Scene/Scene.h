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
		struct TimeStep;
		class UID;
	}
	namespace Graphics
	{
		class EditorCamera;
	}

	class Entity;

	//Document
	class Scene
	{
	public:
		Scene() noexcept = default;
		~Scene() noexcept = default;

		constexpr Scene(const Scene&) = delete;
		Scene(Scene&&) noexcept = default;
		constexpr Scene& operator=(const Scene&) = delete;
		Scene& operator=(Scene&&) noexcept = default;

		static TRAP::Ref<Scene> Copy(const Ref<Scene>& other);

		Entity CreateEntity(const std::string& name = std::string());
		Entity CreateEntityWithUID(Utils::UID uid, const std::string& name = std::string());
		void DestroyEntity(Entity entity);

		void OnRuntimeStart();
		void OnRuntimeStop();

		void OnTickRuntime(const TRAP::Utils::TimeStep& deltaTime);
		void OnUpdateRuntime(Utils::TimeStep deltaTime);
#ifndef TRAP_HEADLESS_MODE
		void OnUpdateEditor(Utils::TimeStep deltaTime, Graphics::EditorCamera& camera);
#endif /*TRAP_HEADLESS_MODE*/
		void OnTick(const TRAP::Utils::TimeStep& deltaTime);
		void OnViewportResize(u32 width, u32 height);

		void DuplicateEntity(Entity entity);

		[[nodiscard]] Entity GetPrimaryCameraEntity();

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
		u32 m_viewportWidth = 0, m_viewportHeight = 0;
	};
}

#endif /*TRAP_SCENE_H*/
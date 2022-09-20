#ifndef TRAP_SCENE_H
#define TRAP_SCENE_H

#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif
#include <entt.hpp>
#ifdef _MSC_VER
	#pragma warning(pop)
#endif

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
		Scene() = default;
		~Scene() = default;

		Scene(const Scene&) = delete;
		Scene(Scene&&) = default;
		Scene& operator=(const Scene&) = delete;
		Scene& operator=(Scene&&) = default;

		Entity CreateEntity(const std::string& name = std::string());
		Entity CreateEntityWithUID(Utils::UID uid, const std::string& name = std::string());
		void DestroyEntity(Entity entity);

		//TEMP
		entt::registry& Reg()
		{
			return m_registry;
		}

		void OnUpdateRuntime(Utils::TimeStep deltaTime);
		void OnUpdateEditor(Utils::TimeStep deltaTime, Graphics::EditorCamera& camera);
		void OnTick();
		void OnViewportResize(uint32_t width, uint32_t height);

		Entity GetPrimaryCameraEntity();

	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneGraphPanel;

		entt::registry m_registry;
		uint32_t m_viewportWidth = 0, m_viewportHeight = 0;
	};
}

#endif /*TRAP_SCENE_H*/
#ifndef _TRAP_SCENE_H_
#define _TRAP_SCENE_H_

#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif
#include <entt.h>
#ifdef _MSC_VER
	#pragma warning(pop)
#endif

namespace TRAP
{
	namespace Utils
	{
		struct TimeStep;
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
		void DestroyEntity(Entity entity);

		//TEMP
		entt::registry& Reg()
		{
			return m_registry;
		}

		void OnUpdate(Utils::TimeStep deltaTime);
		void OnTick();
		void OnViewportResize(uint32_t width, uint32_t height);

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

#endif /*_TRAP_SCENE_H_*/
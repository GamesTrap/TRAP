#ifndef _TRAP_SCENE_H_
#define _TRAP_SCENE_H_

#pragma warning(push, 0)
#include "entt.h"
#pragma warning(pop)

namespace TRAP
{
	namespace Utils
	{
		struct TimeStep;
	}

	class Scene
	{
	public:
		Scene();
		~Scene();
		
		Scene(const Scene&) = delete;
		Scene(Scene&&) = default;
		Scene& operator=(const Scene&) = delete;
		Scene& operator=(Scene&&) = default;

		entt::entity CreateEntity();

		//TEMP
		entt::registry& Reg()
		{
			return m_registry;
		}

		void OnUpdate(Utils::TimeStep deltaTime);
		
	private:
		entt::registry m_registry;
	};
}

#endif /*_TRAP_SCENE_H_*/
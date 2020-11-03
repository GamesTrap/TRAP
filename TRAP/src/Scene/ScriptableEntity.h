#ifndef _TRAP_SCRIPTABLEENTITY_H_
#define _TRAP_SCRIPTABLEENTITY_H_

#include "Entity.h"
#include "Utils/Time/TimeStep.h"

namespace TRAP
{
	//Document
	class ScriptableEntity
	{
	public:
		ScriptableEntity() = default;
		virtual ~ScriptableEntity() = default;
		ScriptableEntity(const ScriptableEntity&) = default;
		ScriptableEntity(ScriptableEntity&&) = default;
		ScriptableEntity& operator=(const ScriptableEntity&) = default;
		ScriptableEntity& operator=(ScriptableEntity&&) = default;

		template<typename T>
		T& GetComponent()
		{
			return m_entity.GetComponent<T>();
		}

	protected:
		virtual void OnCreate() {}
		virtual void OnDestroy() {}
		virtual void OnUpdate(Utils::TimeStep deltaTime) {}
		virtual void OnTick() {}
		
	private:
		Entity m_entity;
		friend class Scene;
	};
}

#endif /*_TRAP_SCRIPTABLEENTITY_H_*/
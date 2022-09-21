#ifndef TRAP_SCRIPTABLEENTITY_H
#define TRAP_SCRIPTABLEENTITY_H

#include "Entity.h"
#include "Utils/Time/TimeStep.h"

namespace TRAP
{
	//TODO Document
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
		virtual void OnUpdate(const Utils::TimeStep&) {}
		virtual void OnTick(const Utils::TimeStep&) {}

	private:
		Entity m_entity;
		friend class Scene;
	};
}

#endif /*TRAP_SCRIPTABLEENTITY_H*/
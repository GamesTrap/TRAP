#ifndef TRAP_SCRIPTABLEENTITY_H
#define TRAP_SCRIPTABLEENTITY_H

#include "Entity.h"

namespace TRAP
{
	//TODO Document
	class ScriptableEntity
	{
	public:
		ScriptableEntity() noexcept = default;
		virtual ~ScriptableEntity() = default;
		ScriptableEntity(const ScriptableEntity&) noexcept = default;
		ScriptableEntity(ScriptableEntity&&) noexcept = default;
		ScriptableEntity& operator=(const ScriptableEntity&) noexcept = default;
		ScriptableEntity& operator=(ScriptableEntity&&) noexcept = default;

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
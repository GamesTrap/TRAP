#ifndef TRAP_SCRIPTABLEENTITY_H
#define TRAP_SCRIPTABLEENTITY_H

#include "Entity.h"

namespace TRAP
{
	//TODO Document
	class ScriptableEntity
	{
	public:
		constexpr ScriptableEntity() noexcept = default;
		constexpr virtual ~ScriptableEntity() = default;
		constexpr ScriptableEntity(const ScriptableEntity&) noexcept = default;
		constexpr ScriptableEntity(ScriptableEntity&&) noexcept = default;
		constexpr ScriptableEntity& operator=(const ScriptableEntity&) noexcept = default;
		constexpr ScriptableEntity& operator=(ScriptableEntity&&) noexcept = default;

		template<typename T>
		[[nodiscard]] T& GetComponent()
		{
			return m_entity.GetComponent<T>();
		}

	protected:
		virtual void OnCreate() {}
		virtual void OnDestroy() {}
		virtual void OnUpdate([[maybe_unused]] const Utils::TimeStep& deltaTime) {}
		virtual void OnTick([[maybe_unused]] const Utils::TimeStep& deltaTime) {}

	private:
		Entity m_entity;
		friend class Scene;
	};
}

#endif /*TRAP_SCRIPTABLEENTITY_H*/
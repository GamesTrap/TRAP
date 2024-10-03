#ifndef TRAP_SCRIPTABLEENTITY_H
#define TRAP_SCRIPTABLEENTITY_H

#include "Entity.h"

namespace TRAP
{
	//TODO Document
	class ScriptableEntity
	{
	public:
		/// @brief Constructor.
		constexpr ScriptableEntity() noexcept = default;
		/// @brief Destructor.
		constexpr virtual ~ScriptableEntity() = default;
		/// @brief Copy constructor.
		constexpr ScriptableEntity(const ScriptableEntity&) noexcept = default;
		/// @brief Move constructor.
		constexpr ScriptableEntity(ScriptableEntity&&) noexcept = default;
		/// @brief Copy assignment operator.
		constexpr ScriptableEntity& operator=(const ScriptableEntity&) noexcept = default;
		/// @brief Move assignment operator.
		constexpr ScriptableEntity& operator=(ScriptableEntity&&) noexcept = default;

		template<typename T>
		[[nodiscard]] T& GetComponent() const
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

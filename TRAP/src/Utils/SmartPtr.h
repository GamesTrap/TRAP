#ifndef TRAP_UTILS_SMARTPTR_H
#define TRAP_UTILS_SMARTPTR_H

#include <memory>

namespace TRAP
{
	/// @brief Wrapper for std::unique_ptr. Related to memory management which is still in planing.
	template<typename T>
	using Scope = std::unique_ptr<T>;
	/// @brief Wrapper for std::make_unique. Related to memory management which is still in planing.
	template<typename T, typename... Args>
	constexpr Scope<T> MakeScope(Args&&... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	/// @brief Wrapper for std::shared_ptr. Related to memory management which is still in planing.
	template<typename T>
	using Ref = std::shared_ptr<T>;
	/// @brief Wrapper for std::make_shared. Related to memory management which is still in planing.
	template<typename T, typename... Args>
	constexpr Ref<T> MakeRef(Args&&... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}

#endif /*TRAP_UTILS_SMARTPTR_H*/

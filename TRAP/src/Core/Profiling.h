#ifndef TRAP_CORE_PROFILING_H
#define TRAP_CORE_PROFILING_H

#include "Core/Types.h"
#include "Utils/Bit.h"
#include "Utils/Enum.h"

//Settings for Profiling (with Tracy)

enum class ProfileSystems : u32
{
	None = 0,

	Events = BIT(0u),
	FileSystem = BIT(1u),
	Graphics = BIT(2u),
	Vulkan = BIT(3u),
	ImageLoader = BIT(4u),
	Input = BIT(5u),
	Layers = BIT(6u),
	Network = BIT(7u),
	Scene = BIT(8u),
	ThreadPool = BIT(9u),
	Utils = BIT(10u),
	Window = BIT(11u),
	WindowingAPI = BIT(12u),
	Verbose = BIT(13u), //Toggles profiling of very trivial functions (i.e. getters, setters, etc.)

	All = Events | FileSystem | Graphics | Vulkan | ImageLoader | Input | Layers |
	      Network | Scene | ThreadPool | Utils | Window | WindowingAPI,
	AllVerbose = Events | FileSystem | Graphics | Vulkan | ImageLoader | Input | Layers |
	             Network | Scene | ThreadPool | Utils | Window | WindowingAPI | Verbose
};
MAKE_ENUM_FLAG(ProfileSystems);

/// @brief Retrieve the TRAP systems to be profiled.
/// @return Systems to profile.
[[nodiscard]] ProfileSystems GetTRAPProfileSystems() noexcept;

/// @brief Set the TRAP systems to profile.
/// @param systems Systems to profile.
void SetTRAPProfileSystems(ProfileSystems systems) noexcept;

//-------------------------------------------------------------------------------------------------------------------//

#ifdef TRACY_ENABLE
//Overloads for new and delete (only used for profiling)

[[nodiscard]] void* operator new(const usize size);
[[nodiscard]] void* operator new(const usize size, const std::nothrow_t&) noexcept;
[[nodiscard]] void* operator new[](const usize size);
[[nodiscard]] void* operator new[](const usize size, const std::nothrow_t&) noexcept;
[[nodiscard]] void* operator new(const usize size, const std::align_val_t alignment);
[[nodiscard]] void* operator new(const usize size, const std::align_val_t alignment, const std::nothrow_t&) noexcept;
[[nodiscard]] void* operator new[](const usize size, const std::align_val_t alignment);
[[nodiscard]] void* operator new[](const usize size, const std::align_val_t alignment, const std::nothrow_t&) noexcept;
void operator delete(void* ptr) noexcept;
void operator delete[](void* ptr) noexcept;
void operator delete(void* ptr, std::align_val_t alignment) noexcept;
void operator delete[](void* ptr, std::align_val_t alignment) noexcept;
#if defined(__GNUC__) && !defined(__clang__)
void operator delete(void* ptr, usize size) noexcept;
#endif /*defined(__GNUC__) && !defined(__clang__)*/
void operator delete[](void* ptr, usize size) noexcept;
void operator delete(void* ptr, usize size, std::align_val_t alignment) noexcept;
void operator delete[](void* ptr, usize size, std::align_val_t alignment) noexcept;
void operator delete(void* ptr, const std::nothrow_t& tag) noexcept;
void operator delete[](void* ptr, const std::nothrow_t& tag) noexcept;
void operator delete(void* ptr, std::align_val_t alignment, const std::nothrow_t& tag) noexcept;
void operator delete[](void* ptr, std::align_val_t alignment, const std::nothrow_t& tag) noexcept;
#endif /*TRACY_ENABLE*/

#endif /*TRAP_CORE_PROFILING_H*/

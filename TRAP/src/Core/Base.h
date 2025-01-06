#ifndef TRAP_CORE_H
#define TRAP_CORE_H

#include "PlatformDetection.h"
#include "TRAP_Assert.h"
#include "Backports.h"
#include "Version.h"
#include "Utils/Bit.h"
#include "Utils/Enum.h"

//-------------------------------------------------------------------------------------------------------------------//

/// @brief Activate this to enable support for texture import/exporting from/to other processes
/// @remark This functionality only works on Windows.
// #define USE_EXTERNAL_MEMORY_EXTENSIONS

//-------------------------------------------------------------------------------------------------------------------//

//Enable graphics validation in debug builds by default.
#if (defined(TRAP_DEBUG) || defined(TRAP_RELWITHDEBINFO)) && !defined(DISABLE_GRAPHICS_DEBUG)
	#define ENABLE_GRAPHICS_DEBUG
	#define VERBOSE_GRAPHICS_DEBUG
#endif /*(TRAP_DEBUG || TRAP_RELWITHDEBINFO) && !DISABLE_GRAPHICS_DEBUG*/

//-------------------------------------------------------------------------------------------------------------------//

//Enable Nsight Aftermath support for debug builds.
#ifdef NSIGHT_AFTERMATH_AVAILABLE
	#if defined(TRAP_DEBUG) || defined(TRAP_RELWITHDEBINFO)
		#define ENABLE_NSIGHT_AFTERMATH
	#endif /*TRAP_DEBUG || TRAP_RELWITHDEBINFO*/
#endif /*NSIGHT_AFTERMATH_AVAILABLE*/

//-------------------------------------------------------------------------------------------------------------------//

//Enable GPU based validation. This creates patched shaders that have additional validation code.
//This can slow things down a lot, especially for applications with lots of pipelines. Time to see the first
//rendered frame may take a long time.
//ENABLE_GRAPHICS_DEBUG must be defined for this to work.
//Vulkan: Only works if VK_EXT_validation_features extension is supported.
//#define ENABLE_GPU_BASED_VALIDATION

//-------------------------------------------------------------------------------------------------------------------//

//Headless mode.
//This macro enables the headless mode
//See https://gamestrap.github.io/TRAP-Docs/pages/gettingstarted.html#differences-between-normal-mode-and-headless-mode
//#define TRAP_HEADLESS_MODE

//-------------------------------------------------------------------------------------------------------------------//

//Single process mode.
//This macro makes the engine only allow one instance of itself to run at a time.
//Any subsequent attempts to run a TRAP application will fail with a message box stating that a
//TRAP application is already running.
// #define ENABLE_SINGLE_PROCESS_ONLY

//-------------------------------------------------------------------------------------------------------------------//

/// @brief TRAP version number created with TRAP_MAKE_VERSION
inline constexpr TRAP::SemanticVersion<0, 11, 69> TRAP_VERSION{};

//-------------------------------------------------------------------------------------------------------------------//

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#endif /*__GNUC__ && !__clang__*/

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

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif /*__GNUC__ && !__clang__*/

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

#endif /*TRAP_CORE_H*/

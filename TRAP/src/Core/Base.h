#ifndef TRAP_CORE_H
#define TRAP_CORE_H

#include <memory>
#include <type_traits>

#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif /*_MSC_VER*/
//Tracy - Profiler
#include <tracy/Tracy.hpp>
#ifdef _MSC_VER
	#pragma warning(pop)
#endif /*_MSC_VER*/

#include "PlatformDetection.h"
#include "TRAP_Assert.h"
#include "Backports.h"

//Set this define to enable RenderDoc layer
//#define USE_RENDER_DOC

//-------------------------------------------------------------------------------------------------------------------//

//Activate this to enable support for texture import/exporting from/to other processes
//Note: Only works on Windows
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
//See https://gamestrap.github.io/TRAP/pages/gettingstarted.html#differences-between-normal-mode-and-headless-mode
//#define TRAP_HEADLESS_MODE

//-------------------------------------------------------------------------------------------------------------------//

//Single process mode.
//This macro makes the engine only allow one instance of itself to run at a time.
//Any subsequent attempts to run a TRAP application will fail with a message box stating that a
//TRAP application is already running.
// #define ENABLE_SINGLE_PROCESS_ONLY

//-------------------------------------------------------------------------------------------------------------------//

//Enable Wayland support on Linuxe
//NOTE: Wayland support is still experimental and not fully implemented.
//Currently X11/Xwayland is preferred over native Wayland.
#ifdef TRAP_PLATFORM_LINUX
	#define ENABLE_WAYLAND_SUPPORT
#endif /*TRAP_PLATFORM_LINUX*/

//-------------------------------------------------------------------------------------------------------------------//

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#endif /*__GNUC__ && !__clang__*/

//Settings for Profiling (with Tracy)

enum class ProfileSystems : u32
{
	Events = 1u << 0u,
	FileSystem = 1u << 1u,
	Graphics = 1u << 2u,
	Vulkan = 1u << 3u,
	ImageLoader = 1u << 4u,
	Input = 1u << 5u,
	Layers = 1u << 6u,
	Network = 1u << 7u,
	Scene = 1u << 8u,
	ThreadPool = 1u << 9u,
	Utils = 1u << 10u,
	Window = 1u << 11u,
	WindowingAPI = 1u << 12u,
	Verbose = 1u << 13u, //Toggles profiling of very trivial functions (i.e. getters, setters, etc.)

	All = Events | FileSystem | Graphics | Vulkan | ImageLoader | Input | Layers |
	      Network | Scene | ThreadPool | Utils | Window | WindowingAPI,
	AllVerbose = Events | FileSystem | Graphics | Vulkan | ImageLoader | Input | Layers |
	             Network | Scene | ThreadPool | Utils | Window | WindowingAPI | Verbose
};

[[nodiscard]] inline constexpr bool operator&(const ProfileSystems lhs, const ProfileSystems rhs) noexcept
{
	return static_cast<bool>(std::to_underlying(lhs) & std::to_underlying(rhs));
}

//Set this macro to specify which systems should be profiled.
[[nodiscard]] inline consteval ProfileSystems TRAP_PROFILE_SYSTEMS() noexcept
{
	return ProfileSystems::All;
}

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif /*__GNUC__ && !__clang__*/

//-------------------------------------------------------------------------------------------------------------------//

/// <summary>
/// Construct a version number.
/// </summary>
/// <param name="major">Major version number.</param>
/// <param name="minor">Minor version number.</param>
/// <param name="patch">Patch version number.</param>
/// <returns>Version number packed into a single u32.</returns>
template<u32 major, u32 minor, u32 patch>
[[nodiscard]] consteval u32 TRAP_MAKE_VERSION()
{
	static_assert(major < 1024, "Major version number must be less than 1024.");
	static_assert(minor < 1024,"Minor version number must be less than 1024.");
	static_assert(patch < 4096, "Patch version number must be less than 4096.");

	return major << 22u | minor << 12u | patch;
}

//-------------------------------------------------------------------------------------------------------------------//

/// <summary>
/// Extract major version from a version number created with TRAP_MAKE_VERSION.
/// </summary>
/// <param name="version">Version number created with TRAP_MAKE_VERSION.</param>
/// <returns>Major version number.</returns>
[[nodiscard]] inline consteval u32 TRAP_VERSION_MAJOR(const u32 version) noexcept
{
	return version >> 22u;
}

//-------------------------------------------------------------------------------------------------------------------//

/// <summary>
/// Extract minor version from a version number created with TRAP_MAKE_VERSION.
/// </summary>
/// <param name="version">Version number created with TRAP_MAKE_VERSION.</param>
/// <returns>Minor version number.</returns>
[[nodiscard]] inline consteval u32 TRAP_VERSION_MINOR(const u32 version) noexcept
{
	return version >> 12u;
}

//-------------------------------------------------------------------------------------------------------------------//

/// <summary>
/// Extract patch version from a version number created with TRAP_MAKE_VERSION.
/// </summary>
/// <param name="version">Version number created with TRAP_MAKE_VERSION.</param>
/// <returns>Patch version number.</returns>
[[nodiscard]] inline consteval u32 TRAP_VERSION_PATCH(const u32 version) noexcept
{
	return version & 0xFFFu;
}

//-------------------------------------------------------------------------------------------------------------------//

/// <summary>
/// TRAP version number created with TRAP_MAKE_VERSION
/// </summary>
inline constexpr u32 TRAP_VERSION = TRAP_MAKE_VERSION<0, 9, 123>();

//-------------------------------------------------------------------------------------------------------------------//

#ifndef MAKE_ENUM_FLAG
#define MAKE_ENUM_FLAG(ENUM_TYPE) \
	[[nodiscard]] inline constexpr ENUM_TYPE operator|(const ENUM_TYPE a, const ENUM_TYPE b) noexcept \
	{ \
		return static_cast<ENUM_TYPE>(std::to_underlying(a) | std::to_underlying(b)); \
	} \
	[[nodiscard]] inline constexpr ENUM_TYPE operator&(const ENUM_TYPE a, const ENUM_TYPE b) noexcept \
	{ \
		return static_cast<ENUM_TYPE>(std::to_underlying(a) & std::to_underlying(b)); \
	} \
	inline constexpr ENUM_TYPE operator|=(ENUM_TYPE& a, const ENUM_TYPE b) noexcept { return a = (a | b); }\
	inline constexpr ENUM_TYPE operator&=(ENUM_TYPE& a, const ENUM_TYPE b) noexcept { return a = (a & b); }
#endif /*MAKE_ENUM_FLAG*/

//-------------------------------------------------------------------------------------------------------------------//

/// <summary>
/// Shift 1 x times.
/// </summary>
/// <typeparam name="T">DataType of variable.</typeparam>
/// <param name="x">Amount to shift.</param>
/// <returns>Shifted value.</returns>
[[nodiscard]] inline constexpr std::unsigned_integral auto BIT(const std::unsigned_integral auto x) noexcept
{
	return decltype(x)(1) << x;
}

//-------------------------------------------------------------------------------------------------------------------//

#ifdef TRACY_ENABLE
//Overloads for new and delete (only used for profiling)
[[nodiscard]] void* operator new(const usize count);
[[nodiscard]] void* operator new[](const usize count);
void operator delete(void* ptr) noexcept;
void operator delete[](void* ptr) noexcept;
void operator delete(void* ptr, usize count) noexcept;
void operator delete[](void* ptr, usize count) noexcept;
#endif /*TRACY_ENABLE*/

//-------------------------------------------------------------------------------------------------------------------//

namespace TRAP
{
	/// <summary>
	/// Wrapper for new & delete.
	/// Related to MemoryManagement which is still in planing.
	/// </summary>
	template<typename T>
	using Scope = std::unique_ptr<T>;
	/// <summary>
	/// Wrapper for new & delete.
	/// Related to MemoryManagement which is still in planing.
	/// </summary>
	template<typename T, typename... Args>
	inline constexpr Scope<T> MakeScope(Args&&... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	/// <summary>
	/// Wrapper for new & delete with reference counting.
	/// Related to MemoryManagement which is still in planing.
	/// </summary>
	template<typename T>
	using Ref = std::shared_ptr<T>;
	/// <summary>
	/// Wrapper for new & delete with reference counting.
	/// Related to MemoryManagement which is still in planing.
	/// </summary>
	template<typename T, typename... Args>
	inline constexpr Ref<T> MakeRef(Args&&... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}

#endif /*TRAP_CORE_H*/
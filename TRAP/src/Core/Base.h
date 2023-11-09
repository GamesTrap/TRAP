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
#include "Version.h"

//Set this define to enable RenderDoc layer
//#define USE_RENDER_DOC

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

/// @brief Shift 1 x times.
/// @param x Amount to shift.
/// @return Shifted value.
[[nodiscard]] inline constexpr std::unsigned_integral auto BIT(const std::unsigned_integral auto x) noexcept
{
	return decltype(x)(1) << x;
}

//-------------------------------------------------------------------------------------------------------------------//

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#endif /*__GNUC__ && !__clang__*/

//Settings for Profiling (with Tracy)

enum class ProfileSystems : u32
{
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

[[nodiscard]] inline constexpr bool operator&(const ProfileSystems lhs, const ProfileSystems rhs) noexcept
{
	return static_cast<bool>(std::to_underlying(lhs) & std::to_underlying(rhs));
}

/// @brief Change this function to specify which systems should be profiled.
/// @return Systems to profile.
[[nodiscard]] inline consteval ProfileSystems TRAP_PROFILE_SYSTEMS() noexcept
{
	return ProfileSystems::All;
}

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif /*__GNUC__ && !__clang__*/

//-------------------------------------------------------------------------------------------------------------------//

/// @brief TRAP version number created with TRAP_MAKE_VERSION
inline constexpr TRAP::SemanticVersion<0, 10, 3> TRAP_VERSION{};

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
	/// @brief Wrapper for std::unique_ptr. Related to memory management which is still in planing.
	template<typename T>
	using Scope = std::unique_ptr<T>;
	/// @brief Wrapper for std::make_unique. Related to memory management which is still in planing.
	template<typename T, typename... Args>
	inline constexpr Scope<T> MakeScope(Args&&... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	/// @brief Wrapper for std::shared_ptr. Related to memory management which is still in planing.
	template<typename T>
	using Ref = std::shared_ptr<T>;
	/// @brief Wrapper for std::make_shared. Related to memory management which is still in planing.
	template<typename T, typename... Args>
	inline constexpr Ref<T> MakeRef(Args&&... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}

#endif /*TRAP_CORE_H*/
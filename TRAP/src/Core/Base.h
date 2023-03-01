#ifndef TRAP_CORE_H
#define TRAP_CORE_H

#include <memory>
#include <type_traits>

#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif
//Tracy - Profiler
#include <tracy/Tracy.hpp>
#ifdef _MSC_VER
	#pragma warning(pop)
#endif

#include "PlatformDetection.h"

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
#endif

//-------------------------------------------------------------------------------------------------------------------//

//Enable Nsight Aftermath support for debug builds.
#ifdef NSIGHT_AFTERMATH_AVAILABLE
	#if defined(TRAP_DEBUG) || defined(TRAP_RELWITHDEBINFO)
		#define ENABLE_NSIGHT_AFTERMATH
	#endif
#endif

//-------------------------------------------------------------------------------------------------------------------//

//Enable GPU based validation. This creates patched shaders that have additional validation code.
//This can slow things down a lot, especially for applications with lots of pipelines. Time to see the first
//rendered frame may take a long time.
//ENABLE_GRAPHICS_DEBUG must be defined for this to work.
//Vulkan: Only works if VK_EXT_validation_features extension is supported.
//#define ENABLE_GPU_BASED_VALIDATION

//-------------------------------------------------------------------------------------------------------------------//

//Headless mode.
//This macro changes Window and RendererAPI behaviour.
//By default the Main Window will be hidden.
//The RendererAPI uses relaxed requirements to allow for offline rendering or compute
//if the hardware supports an RenderAPI.
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
	// #define ENABLE_WAYLAND_SUPPORT
#endif

//-------------------------------------------------------------------------------------------------------------------//

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#endif

//Settings for Profiling (with Tracy)

enum class ProfileSystems
{
	Events = 1 << 0,
	FileSystem = 1 << 1,
	Graphics = 1 << 2,
	Vulkan = 1 << 3,
	ImageLoader = 1 << 4,
	Input = 1 << 5,
	Layers = 1 << 6,
	Network = 1 << 7,
	Scene = 1 << 8,
	ThreadPool = 1 << 9,
	Utils = 1 << 10,
	Window = 1 << 11,
	WindowingAPI = 1 << 12,
	Verbose = 1 << 13, //Toggles profiling of very trivial functions (i.e. getters, setters, etc.)

	All = Events | FileSystem | Graphics | Vulkan | ImageLoader | Input | Layers |
	      Network | Scene | ThreadPool | Utils | Window | WindowingAPI,
	AllVerbose = Events | FileSystem | Graphics | Vulkan | ImageLoader | Input | Layers |
	             Network | Scene | ThreadPool | Utils | Window | WindowingAPI | Verbose
};

[[nodiscard]] constexpr bool operator&(const ProfileSystems lhs, const ProfileSystems rhs) noexcept
{
	return static_cast<bool>(static_cast<std::underlying_type_t<ProfileSystems>>(lhs) &
			                 static_cast<std::underlying_type_t<ProfileSystems>>(rhs));
}

//Set this macro to specify which systems should be profiled.
[[nodiscard]] constexpr ProfileSystems TRAP_PROFILE_SYSTEMS() noexcept
{
	return ProfileSystems::All;
}

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif

//-------------------------------------------------------------------------------------------------------------------//

/// <summary>
/// Construct a version number.
/// </summary>
/// <param name="major">Major version number.</param>
/// <param name="minor">Minor version number.</param>
/// <param name="patch">Patch version number.</param>
/// <returns>Version number packed into a single uint32_t.</returns>
[[nodiscard]] uint32_t TRAP_MAKE_VERSION(const uint32_t major, const uint32_t minor, const uint32_t patch);

//-------------------------------------------------------------------------------------------------------------------//

/// <summary>
/// Extract major version from a version number created with TRAP_MAKE_VERSION.
/// </summary>
/// <param name="version">Version number created with TRAP_MAKE_VERSION.</param>
/// <returns>Major version number.</returns>
[[nodiscard]] constexpr uint32_t TRAP_VERSION_MAJOR(const uint32_t version) noexcept
{
	return version >> 22u;
}

//-------------------------------------------------------------------------------------------------------------------//

/// <summary>
/// Extract minor version from a version number created with TRAP_MAKE_VERSION.
/// </summary>
/// <param name="version">Version number created with TRAP_MAKE_VERSION.</param>
/// <returns>Minor version number.</returns>
[[nodiscard]] constexpr uint32_t TRAP_VERSION_MINOR(const uint32_t version) noexcept
{
	return version >> 12u;
}

//-------------------------------------------------------------------------------------------------------------------//

/// <summary>
/// Extract patch version from a version number created with TRAP_MAKE_VERSION.
/// </summary>
/// <param name="version">Version number created with TRAP_MAKE_VERSION.</param>
/// <returns>Patch version number.</returns>
[[nodiscard]] constexpr uint32_t TRAP_VERSION_PATCH(const uint32_t version) noexcept
{
	return version & 0xFFFu;
}

//-------------------------------------------------------------------------------------------------------------------//

/// <summary>
/// TRAP version number created with TRAP_MAKE_VERSION
/// </summary>
const uint32_t TRAP_VERSION = TRAP_MAKE_VERSION(0, 8, 99);

//-------------------------------------------------------------------------------------------------------------------//

#ifndef MAKE_ENUM_FLAG
#define MAKE_ENUM_FLAG(ENUM_TYPE) \
	constexpr inline ENUM_TYPE operator|(const ENUM_TYPE a, const ENUM_TYPE b) noexcept \
	{ \
		return static_cast<ENUM_TYPE>(static_cast<std::underlying_type<ENUM_TYPE>::type>(a) | \
		 							  static_cast<std::underlying_type<ENUM_TYPE>::type>(b)); \
	} \
	constexpr inline ENUM_TYPE operator&(const ENUM_TYPE a, const ENUM_TYPE b) noexcept \
	{ \
		return static_cast<ENUM_TYPE>(static_cast<std::underlying_type<ENUM_TYPE>::type>(a) & \
									  static_cast<std::underlying_type<ENUM_TYPE>::type>(b)); \
	} \
	constexpr inline ENUM_TYPE operator|=(ENUM_TYPE& a, const ENUM_TYPE b) noexcept { return a = (a | b); }\
	constexpr inline ENUM_TYPE operator&=(ENUM_TYPE& a, const ENUM_TYPE b) noexcept { return a = (a & b); }
#endif

//-------------------------------------------------------------------------------------------------------------------//

#if defined(TRAP_DEBUG) || defined(TRAP_RELWITHDEBINFO)
	#if defined(TRAP_PLATFORM_WINDOWS)
		/// <summary>
		/// Sets a cross platform debug break.
		/// Note: Only works when TRAP_DEBUG or TRAP_RELWITHDEBINFO is set.
		/// </summary>
		inline void TRAP_DEBUG_BREAK()
		{
			__debugbreak();
		}
	#elif defined(TRAP_PLATFORM_LINUX)
		/// <summary>
		/// Sets a cross platform debug break.
		/// Note: Only works when TRAP_DEBUG or TRAP_RELWITHDEBINFO is set.
		/// </summary>
		#include <signal.h>
		inline void TRAP_DEBUG_BREAK()
		{
			raise(SIGTRAP);
		}
	#else
		/// <summary>
		/// Sets a cross platform debug break.
		/// Note: Only works when TRAP_DEBUG or TRAP_RELWITHDEBINFO is set.
		/// </summary>
		constexpr void TRAP_DEBUG_BREAK() noexcept
		{}
	#endif
#else
		/// <summary>
		/// Sets a cross platform debug break.
		/// Note: Only works when TRAP_DEBUG or TRAP_RELWITHDEBINFO is set.
		/// </summary>
		constexpr void TRAP_DEBUG_BREAK() noexcept
		{}
#endif

//-------------------------------------------------------------------------------------------------------------------//

#define TRAP_EXPAND_MACRO(x) x
#define TRAP_STRINGIFY_MACRO(x) #x

#include "Log/Log.h"

/// <summary>
/// Shift 1 x times.
/// </summary>
/// <typeparam name="T">DataType of variable.</typeparam>
/// <param name="x">Amount to shift.</param>
/// <returns>Shifted value.</returns>
template <typename T>
[[nodiscard]] constexpr T BIT(const T x) noexcept
{
	return T(1) << x;
}

//-------------------------------------------------------------------------------------------------------------------//

#ifdef TRACY_ENABLE
//Overloads for new and delete (only used for profiling)
[[nodiscard]] void* operator new(const std::size_t count);
[[nodiscard]] void* operator new[](const std::size_t count);
void operator delete(void* ptr) noexcept;
void operator delete[](void* ptr) noexcept;
void operator delete(void* ptr, std::size_t count) noexcept;
void operator delete[](void* ptr, std::size_t count) noexcept;
#endif /*TRACY_ENABLE*/

//-------------------------------------------------------------------------------------------------------------------//

//TODO Temporary, remove after Tracy fixed this
#ifndef TRACY_ENABLE
#ifdef TracyLockable
#undef TracyLockable
#endif /*TracyLockable*/
#define TracyLockable( type, varname ) type varname
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
	constexpr Scope<T> MakeScope(Args&&... args)
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
	constexpr Ref<T> MakeRef(Args&&... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}

#endif /*TRAP_CORE_H*/
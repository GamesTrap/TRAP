#ifndef TRAP_CORE_H
#define TRAP_CORE_H

#include <memory>
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
// #define ENABLE_GPU_BASED_VALIDATION

//-------------------------------------------------------------------------------------------------------------------//

//Headless mode.
//This macro changes Window and RendererAPI behaviour.
//By default the Main Window will be hidden.
//The RendererAPI uses relaxed requirements to allow for offline rendering or compute
//if the hardware supports an RenderAPI.
//#define TRAP_HEADLESS_MODE

//-------------------------------------------------------------------------------------------------------------------//

/// <summary>
/// Construct a version number.
/// </summary>
/// <param name="major">Major version number.</param>
/// <param name="minor">Minor version number.</param>
/// <param name="patch">Patch version number.</param>
/// <returns>Version number packed into a single uint32_t.</returns>
constexpr uint32_t TRAP_MAKE_VERSION(const uint32_t major, const uint32_t minor, const uint32_t patch)
{
	return major << 22 | minor << 12 | patch;
}

//-------------------------------------------------------------------------------------------------------------------//

/// <summary>
/// Extract major version from a version number created with TRAP_MAKE_VERSION.
/// </summary>
/// <param name="version">Version number created with TRAP_MAKE_VERSION.</param>
/// <returns>Major version number.</returns>
constexpr uint32_t TRAP_VERSION_MAJOR(const uint32_t version)
{
	return static_cast<uint32_t>(version) >> 22;
}

//-------------------------------------------------------------------------------------------------------------------//

/// <summary>
/// Extract minor version from a version number created with TRAP_MAKE_VERSION.
/// </summary>
/// <param name="version">Version number created with TRAP_MAKE_VERSION.</param>
/// <returns>Minor version number.</returns>
constexpr uint32_t TRAP_VERSION_MINOR(const uint32_t version)
{
	return static_cast<uint32_t>(version) >> 12;
}

//-------------------------------------------------------------------------------------------------------------------//

/// <summary>
/// Extract patch version from a version number created with TRAP_MAKE_VERSION.
/// </summary>
/// <param name="version">Version number created with TRAP_MAKE_VERSION.</param>
/// <returns>Patch version number.</returns>
constexpr uint32_t TRAP_VERSION_PATCH(const uint32_t version)
{
	return static_cast<uint32_t>(version) & 0xFFF;
}

//-------------------------------------------------------------------------------------------------------------------//

/// <summary>
/// TRAP version number created with TRAP_MAKE_VERSION
/// </summary>
constexpr uint32_t TRAP_VERSION = TRAP_MAKE_VERSION(0, 7, 137);

//-------------------------------------------------------------------------------------------------------------------//

#ifndef MAKE_ENUM_FLAG
#define MAKE_ENUM_FLAG(ENUM_TYPE) \
	static constexpr inline ENUM_TYPE operator|(ENUM_TYPE a, ENUM_TYPE b) \
	{ \
		return static_cast<ENUM_TYPE>(static_cast<std::underlying_type<ENUM_TYPE>::type>(a) | \
		 							  static_cast<std::underlying_type<ENUM_TYPE>::type>(b)); \
	} \
	static constexpr inline ENUM_TYPE operator&(ENUM_TYPE a, ENUM_TYPE b) \
	{ \
		return static_cast<ENUM_TYPE>(static_cast<std::underlying_type<ENUM_TYPE>::type>(a) & \
									  static_cast<std::underlying_type<ENUM_TYPE>::type>(b)); \
	} \
	static constexpr inline ENUM_TYPE operator|=(ENUM_TYPE& a, ENUM_TYPE b) { return a = (a | b); }\
	static constexpr inline ENUM_TYPE operator&=(ENUM_TYPE& a, ENUM_TYPE b) { return a = (a & b); }
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
		constexpr void TRAP_DEBUG_BREAK()
		{
		}
	#endif
#else
		/// <summary>
		/// Sets a cross platform debug break.
		/// Note: Only works when TRAP_DEBUG or TRAP_RELWITHDEBINFO is set.
		/// </summary>
		constexpr void TRAP_DEBUG_BREAK()
		{
		}
#endif

//-------------------------------------------------------------------------------------------------------------------//

#define TRAP_EXPAND_MACRO(x) x
#define TRAP_STRINGIFY_MACRO(x) #x

#include "TRAP_Assert.h"
#include "Log/Log.h"

/// <summary>
/// Shift 1 x times.
/// </summary>
/// <typeparam name="T">DataType of variable.</typeparam>
/// <param name="x">Amount to shift.</param>
/// <returns>Shifted value.</returns>
template <typename T>
constexpr T BIT(T x)
{
	return 1 << x;
}

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
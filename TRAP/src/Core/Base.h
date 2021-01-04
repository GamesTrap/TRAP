#ifndef _TRAP_CORE_H_
#define _TRAP_CORE_H_

#include <memory>

//Set this define to enable renderdoc layer
//NOTE: Settings this define will disable use of the KHR dedicated allocation extension since
//      it conflicts with the renderdoc capture layer.
//#define USE_RENDER_DOC

//Enable graphics validation in debug builds by default.
#if (defined(TRAP_DEBUG) || defined(TRAP_RELWITHDEBINFO)) && !defined(DISABLE_GRAPHICS_DEBUG)
	#define ENABLE_GRAPHICS_DEBUG
	#define VERBOSE_GRAPHICS_DEBUG
#endif

/// <summary>
/// Construct an API/Engine version number.
/// </summary>
/// <param name="major">Major version number.</param>
/// <param name="minor">Minor version number.</param>
/// <param name="patch">Patch version number.</param>
/// <returns>API/Engine version number packed into a single uint32_t.</returns>
constexpr uint32_t TRAP_MAKE_VERSION(const uint32_t major, const uint32_t minor, const uint32_t patch)
{
	return major << 22 | minor << 12 | patch;
}

//-------------------------------------------------------------------------------------------------------------------//

/// <summary>
/// Extract major version number from API/Engine version created with TRAP_MAKE_VERSION.
/// </summary>
/// <param name="version">API/Engine version created with TRAP_MAKE_VERSION.</param>
/// <returns>Major version number.</returns>
constexpr uint32_t TRAP_VERSION_MAJOR(const uint32_t version)
{
	return static_cast<uint32_t>(version) >> 22;
}

//-------------------------------------------------------------------------------------------------------------------//

/// <summary>
/// Extract minor version number from API/Engine version created with TRAP_MAKE_VERSION.
/// </summary>
/// <param name="version">API/Engine version created with TRAP_MAKE_VERSION.</param>
/// <returns>Minor version number.</returns>
constexpr uint32_t TRAP_VERSION_MINOR(const uint32_t version)
{
	return static_cast<uint32_t>(version) >> 12;
}

//-------------------------------------------------------------------------------------------------------------------//

/// <summary>
/// Extract patch version number from API/Engine version created with TRAP_MAKE_VERSION.
/// </summary>
/// <param name="version">API/Engine version created with TRAP_MAKE_VERSION.</param>
/// <returns>Patch version number.</returns>
constexpr uint32_t TRAP_VERSION_PATCH(const uint32_t version)
{
	return static_cast<uint32_t>(version) & 0xFFF;
}

//-------------------------------------------------------------------------------------------------------------------//

/// <summary>
/// TRAP_VERSION number created with TRAP_MAKE_VERSION
/// </summary>
constexpr uint32_t TRAP_VERSION = TRAP_MAKE_VERSION(0, 7, 15);

//-------------------------------------------------------------------------------------------------------------------//

#ifndef MAKE_ENUM_FLAG
#define MAKE_ENUM_FLAG(ENUM_TYPE)\
	static inline ENUM_TYPE operator|(ENUM_TYPE a, ENUM_TYPE b) { return static_cast<ENUM_TYPE>(static_cast<std::underlying_type<ENUM_TYPE>::type>(a) | \
																		                        static_cast<std::underlying_type<ENUM_TYPE>::type>(b)); } \
	static inline ENUM_TYPE operator&(ENUM_TYPE a, ENUM_TYPE b) { return static_cast<ENUM_TYPE>(static_cast<std::underlying_type<ENUM_TYPE>::type>(a) & \
																		                        static_cast<std::underlying_type<ENUM_TYPE>::type>(b)); } \
	static inline ENUM_TYPE operator|=(ENUM_TYPE& a, ENUM_TYPE b) { return a = (a | b); }\
	static inline ENUM_TYPE operator&=(ENUM_TYPE& a, ENUM_TYPE b) { return a = (a & b); }
#endif

//-------------------------------------------------------------------------------------------------------------------//

#if defined(TRAP_DEBUG) || defined(TRAP_RELWITHDEBINFO)
	#ifdef TRAP_PLATFORM_WINDOWS
		/// <summary>
		/// Set a cross platform Debug Break.<br>
		/// Note: Only works when TRAP_DEBUG or TRAP_RELWITHDEBINFO is set.
		/// </summary>
		inline void TRAP_DEBUG_BREAK()
		{
			__debugbreak();
		}
	#elif defined(TRAP_PLATFORM_LINUX)
		/// <summary>
		/// Set a cross platform Debug Break.<br>
		/// Note: Only works when TRAP_DEBUG or TRAP_RELWITHDEBINFO is set.
		/// </summary>
		#include <signal.h>	
		inline void TRAP_DEBUG_BREAK()
		{
			raise(SIGTRAP);
		}
	#else
		/// <summary>
		/// Set a cross platform Debug Break.<br>
		/// Note: Only works when TRAP_DEBUG or TRAP_RELWITHDEBINFO is set.
		/// </summary>
		constexpr void TRAP_DEBUG_BREAK()
		{			
		}
	#endif
#else
		/// <summary>
		/// Set a cross platform Debug Break.<br>
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
	/// WIP!<br>
	/// Related to MemoryManagement which is still in planing.
	/// </summary>
	template<typename T>
	using Scope = std::unique_ptr<T>;
	/// <summary>
	/// WIP!<br>
	/// Related to MemoryManagement which is still in planing.
	/// </summary>
	template<typename T, typename... Args>
	constexpr Scope<T> MakeScope(Args&&... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);		
	}

	/// <summary>
	/// WIP!<br>
	/// Related to MemoryManagement which is still in planing.
	/// </summary>
	template<typename T>
	using Ref = std::shared_ptr<T>;
	/// <summary>
	/// WIP!<br>
	/// Related to MemoryManagement which is still in planing.
	/// </summary>
	template<typename T, typename... Args>
	constexpr Ref<T> MakeRef(Args&&... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}

#endif /*_TRAP_CORE_H_*/
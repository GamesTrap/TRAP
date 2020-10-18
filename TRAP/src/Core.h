#ifndef _TRAP_CORE_H_
#define _TRAP_CORE_H_

#ifdef _WIN32
	//Windows x64/x86
	#ifdef _WIN64
		//Windows x64
		#define TRAP_PLATFORM_WINDOWS
	#else
		//Windows x86
		#error "x86 Builds aren unsupported!"
	#endif
#elif defined(__APPLE__) || defined(__MACH__)
	#error "macOS is unsupported!"
//We also have to check __ANDROID__ before __linux__ since android is based on the linux kernel it has __linux__ defined
#elif defined(__ANDROID__)
	#error "Android is unsupported!" //Maybe in the future
#elif defined(__linux__)
	#define TRAP_PLATFORM_LINUX
#else
	#error "Unknown platform!"
#endif

//-------------------------------------------------------------------------------------------------------------------//

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
constexpr uint32_t TRAP_VERSION = TRAP_MAKE_VERSION(0, 6, 77);

//-------------------------------------------------------------------------------------------------------------------//

#if defined(TRAP_DEBUG) || defined(TRAP_RELWITHDEBINFO)
	#ifdef TRAP_PLATFORM_WINDOWS
		/// <summary>
		/// Set a cross platform Debug Break.<br>
		/// Note: Only works when TRAP_DEBUG or TRAP_RELWITHDEBINFO is set.
		/// </summary>
		void TRAP_DEBUG_BREAK();
	#elif defined(TRAP_PLATFORM_LINUX)
		/// <summary>
		/// Set a cross platform Debug Break.<br>
		/// Note: Only works when TRAP_DEBUG or TRAP_RELWITHDEBINFO is set.
		/// </summary>
		void TRAP_DEBUG_BREAK();
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

#if defined(TRAP_DEBUG) || defined(TRAP_RELWITHDEBINFO)
	#define TRAP_ENABLE_ASSERTS
#endif

#define TRAP_EXPAND_MACRO(x) x
#define TRAP_STRINGIFY_MACRO(x) #x

#include "Log/Log.h"
#include "TRAP_Assert.h"

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
	/// <typeparam name="T"></typeparam>
	template<typename T>
	using Scope = std::unique_ptr<T>;
	/// <summary>
	/// WIP!<br>
	/// Related to MemoryManagement which is still in planing.
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <typeparam name="...Args"></typeparam>
	/// <param name="...args"></param>
	/// <returns></returns>
	template<typename T, typename... Args>
	constexpr Scope<T> MakeScope(Args&&... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);		
	}

	/// <summary>
	/// WIP!<br>
	/// Related to MemoryManagement which is still in planing.
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template<typename T>
	using Ref = std::shared_ptr<T>;
	/// <summary>
	/// WIP!<br>
	/// Related to MemoryManagement which is still in planing.
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <typeparam name="...Args"></typeparam>
	/// <param name="...args"></param>
	/// <returns></returns>
	template<typename T, typename... Args>
	constexpr Ref<T> MakeRef(Args&&... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}

#endif /*_TRAP_CORE_H_*/
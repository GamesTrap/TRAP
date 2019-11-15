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

constexpr uint32_t TRAP_MAKE_VERSION(const uint32_t major, const uint32_t minor, const uint32_t patch)
{
	return major << 22 | minor << 12 | patch;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr uint32_t TRAP_VERSION_MAJOR(const uint32_t version)
{
	return static_cast<uint32_t>(version) >> 22;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr uint32_t TRAP_VERSION_MINOR(const uint32_t version)
{
	return static_cast<uint32_t>(version) >> 12;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr uint32_t TRAP_VERSION_PATCH(const uint32_t version)
{
	return static_cast<uint32_t>(version) & 0xFFF;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr uint32_t TRAP_VERSION = TRAP_MAKE_VERSION(0, 5, 66);

//-------------------------------------------------------------------------------------------------------------------//

#if defined(TRAP_DEBUG) || defined(TRAP_RELWITHDEBINFO)
	#ifdef TRAP_PLATFORM_WINDOWS
		void TRAP_DEBUG_BREAK();
	#elif defined(TRAP_PLATFORM_LINUX)
		#include <signal.h>		
		void TRAP_DEBUG_BREAK();
	#else
		constexpr void TRAP_DEBUG_BREAK()
		{			
		}
	#endif
#else
		constexpr void TRAP_DEBUG_BREAK()
		{			
		}
#endif

//-------------------------------------------------------------------------------------------------------------------//

#if defined(TRAP_DEBUG) || defined(TRAP_RELWITHDEBINFO)
	#define TRAP_ENABLE_ASSERTS
#endif

#include "Log/Log.h"

#ifdef TRAP_ENABLE_ASSERTS

//-------------------------------------------------------------------------------------------------------------------//

#define TRAP_ASSERT(x, ...)                                                                     \
	{                                                                                         \
		if (!(x))                                                                             \
		{                                                                                     \
			TP_ERROR("Assertion Failed: ", __VA_ARGS__, " @[", __FILE__, ':', __LINE__, ']'); \
			TRAP_DEBUG_BREAK();                                                                 \
		}                                                                                     \
	}

//-------------------------------------------------------------------------------------------------------------------//

#define TRAP_CORE_ASSERT(x, ...)                                                                       \
	{                                                                                                \
		if (!(x))                                                                                    \
		{                                                                                            \
			TP_ERROR("[Core] Assertion Failed: ", __VA_ARGS__, " @[", __FILE__, ':', __LINE__, ']'); \
			TRAP_DEBUG_BREAK();                                                                        \
		}                                                                                            \
	}

//-------------------------------------------------------------------------------------------------------------------//

#define TRAP_RENDERER_ASSERT(x, ...)                                                                       \
	{                                                                                                    \
		if (!(x))                                                                                        \
		{                                                                                                \
			TP_ERROR("[Renderer] Assertion Failed: ", __VA_ARGS__, " @[", __FILE__, ':', __LINE__, ']'); \
			TRAP_DEBUG_BREAK();                                                                            \
		}                                                                                                \
	}

//-------------------------------------------------------------------------------------------------------------------//

#else
template <typename T, typename... Args>
constexpr void TRAP_ASSERT(T x, const Args &... args)
{
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T, typename... Args>
constexpr void TRAP_CORE_ASSERT(T x, const Args &... args)
{
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T, typename... Args>
constexpr void TRAP_RENDERER_ASSERT(T x, const Args &... args)
{
}
#endif

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
constexpr T BIT(T x)
{
	return 1 << x;
}

//-------------------------------------------------------------------------------------------------------------------//

#define TRAP_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

namespace TRAP
{
	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename... Args>
	constexpr Scope<T> MakeScope(Args&&... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);		
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename... Args>
	constexpr Ref<T> MakeRef(Args&&... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}

#endif /*_TRAP_CORE_H_*/
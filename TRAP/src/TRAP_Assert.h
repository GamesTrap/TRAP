#ifndef TRAP_TRAPASSERT_H
#define TRAP_TRAPASSERT_H

#include "Core/PlatformDetection.h"
#include "Log/Log.h"
#include <filesystem>

#if defined(TRAP_DEBUG) || defined(TRAP_RELWITHDEBINFO)
	#define TRAP_ENABLE_ASSERTS
#endif /*TRAP_DEBUG || TRAP_RELWITHDEBINFO*/

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
		inline constexpr void TRAP_DEBUG_BREAK() noexcept
		{}
	#endif
#else
		/// <summary>
		/// Sets a cross platform debug break.
		/// Note: Only works when TRAP_DEBUG or TRAP_RELWITHDEBINFO is set.
		/// </summary>
		inline constexpr void TRAP_DEBUG_BREAK() noexcept
		{}
#endif /*TRAP_DEBUG || TRAP_RELWITHDEBINFO*/

#define TRAP_EXPAND_MACRO(x) x
#define TRAP_STRINGIFY_MACRO(x) #x

#ifdef TRAP_ENABLE_ASSERTS

template<typename... Args>
void TRAP_ASSERT_IMPL_LOG(const std::string_view expressionStr, const std::string_view file, const uint64_t line, [[maybe_unused]] const Args... args)
{
	if constexpr(sizeof...(Args) > 1)
	{
		TP_CRITICAL("Assertion '", expressionStr, "' failed: ", args..., " @ ", std::filesystem::absolute(file).string(), ':', line);
	}
	else
	{
		TP_CRITICAL("Assertion '", expressionStr, "' failed @ ", std::filesystem::absolute(file).string(), ':', line);
	}
}

#define TRAP_ASSERT_IMPL(check, ...) { if(!(check)) { TRAP_ASSERT_IMPL_LOG(TRAP_STRINGIFY_MACRO(check), __FILE__, __LINE__, __VA_ARGS__); TRAP_DEBUG_BREAK(); } }

//Currently accepts at least the condition and one additional parameter (the message) being optional
#define TRAP_ASSERT(...) TRAP_EXPAND_MACRO(TRAP_ASSERT_IMPL(__VA_ARGS__, ""))
#else
template<typename... Args>
constexpr void TRAP_ASSERT(const Args&...) {}
#endif

#endif /*TRAP_TRAPASSERT_H*/
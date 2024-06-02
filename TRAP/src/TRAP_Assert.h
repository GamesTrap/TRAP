#ifndef TRAP_TRAPASSERT_H
#define TRAP_TRAPASSERT_H

#include "Core/PlatformDetection.h"
#include "Core/Backports.h"
#include "Log/Log.h"

#include <source_location>

#if (defined(TRAP_DEBUG) || defined(TRAP_RELWITHDEBINFO)) && !defined(TRAP_UNITTESTS)
	#define TRAP_ENABLE_ASSERTS
#endif /*TRAP_DEBUG || TRAP_RELWITHDEBINFO*/

//-------------------------------------------------------------------------------------------------------------------//

#define TRAP_EXPAND_MACRO(x) x
#define TRAP_STRINGIFY_MACRO(x) #x

#ifdef TRAP_ENABLE_ASSERTS

template<typename... Args>
constexpr void TRAP_ASSERT_IMPL_LOG(const std::string_view expressionStr,
                                    const std::source_location& sourceLoc,
                                    [[maybe_unused]] const Args&... args)
{
	if (!std::is_constant_evaluated())
	{
		if constexpr(sizeof...(Args) > 1)
		{
			TP_CRITICAL(" Assertion '", expressionStr, "' failed: \"", args..., "\" in ", sourceLoc);
		}
		else
		{
			TP_CRITICAL(" Assertion '", expressionStr, "' failed in ", sourceLoc);
		}
	}
}

#define TRAP_ASSERT_IMPL(check, ...)                                                  \
    {                                                                                 \
		if(!std::is_constant_evaluated())                                             \
		{                                                                             \
			if(!(check))                                                              \
			{                                                                         \
				constexpr std::source_location loc = std::source_location::current(); \
				TRAP_ASSERT_IMPL_LOG(TRAP_STRINGIFY_MACRO(check), loc, __VA_ARGS__);  \
				if (std::is_constant_evaluated())                                     \
				{                                                                     \
					std::breakpoint_if_debugging();                                   \
				}                                                                     \
			}                                                                         \
		}                                                                             \
	}

/// @brief Assertion macro.
///        Requires conditions as first parameter.
///        Optionally provide a description to be logged.
/// @note Inside constant evaluation context this turns into a No-Op to allow usage in constexpr functions.
#define TRAP_ASSERT(...) TRAP_EXPAND_MACRO(TRAP_ASSERT_IMPL(__VA_ARGS__, ""))
#else
constexpr void TRAP_ASSERT(const auto&...) {}
#endif

#endif /*TRAP_TRAPASSERT_H*/

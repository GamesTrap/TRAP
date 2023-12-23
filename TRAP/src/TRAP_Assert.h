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
void TRAP_ASSERT_IMPL_LOG(const std::string_view expressionStr, const std::string_view filename,
                          const std::string_view function, const std::uint_least32_t line,
						  const std::uint_least32_t column, [[maybe_unused]] const Args&... args)
{
	if constexpr(sizeof...(Args) > 1)
	{
		TP_CRITICAL("Assertion '", expressionStr, "' failed: \"", args..., "\" in ", filename, " @ ", function, ':', line, ':', column);
	}
	else
	{
		TP_CRITICAL("Assertion '", expressionStr, "' failed in ", filename, " @ ", function, ':', line, ':', column);
	}
}

#define TRAP_ASSERT_IMPL(check, ...) { if(!(check)) { constexpr std::source_location loc = std::source_location::current(); \
                                                      TRAP_ASSERT_IMPL_LOG(TRAP_STRINGIFY_MACRO(check), loc.file_name(), loc.function_name(), loc.line(), loc.column(), __VA_ARGS__); \
													  std::breakpoint_if_debugging(); } }

//Currently accepts at least the condition and one additional parameter (the message) being optional
#define TRAP_ASSERT(...) TRAP_EXPAND_MACRO(TRAP_ASSERT_IMPL(__VA_ARGS__, ""))
#else
constexpr void TRAP_ASSERT(const auto&...) {}
#endif

#endif /*TRAP_TRAPASSERT_H*/
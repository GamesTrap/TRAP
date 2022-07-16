#ifndef TRAP_TRAPASSERT_H
#define TRAP_TRAPASSERT_H

#include "Core/Base.h"
#include "Log/Log.h"
#include <filesystem>

#if defined(TRAP_DEBUG) || defined(TRAP_RELWITHDEBINFO)
	#define TRAP_ENABLE_ASSERTS
#endif

#ifdef TRAP_ENABLE_ASSERTS

#define TRAP_INTERNAL_ASSERT_IMPL(check, msg, ...) { if(!(check)) { TP_CRITICAL(msg, __VA_ARGS__);                   \
                                                                    TRAP_DEBUG_BREAK(); } }

#define TRAP_INTERNAL_ASSERT_WITH_MSG(check, ...) TRAP_INTERNAL_ASSERT_IMPL                                          \
	(                                                                                                                \
		check, " Assertion '", TRAP_STRINGIFY_MACRO(check), "' failed: ", __VA_ARGS__, " @ ",                        \
		std::filesystem::absolute(std::filesystem::path(__FILE__)).u8string(), ':', __LINE__                         \
	)
#define TRAP_INTERNAL_ASSERT_NO_MSG(check) TRAP_INTERNAL_ASSERT_IMPL                                                 \
	(                                                                                                                \
		check, " Assertion '", TRAP_STRINGIFY_MACRO(check)"' failed @ ",                                             \
		std::filesystem::absolute(std::filesystem::path(__FILE__)).u8string(), ':', __LINE__                         \
	)

#define TRAP_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
#define TRAP_INTERNAL_ASSERT_GET_MACRO(...) TRAP_EXPAND_MACRO                                                        \
	(                                                                                                                \
		TRAP_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, TRAP_INTERNAL_ASSERT_WITH_MSG, TRAP_INTERNAL_ASSERT_NO_MSG) \
	)

//Currently accepts at least the condition and one additional parameter (the message) being optional
#define TRAP_ASSERT(...) TRAP_EXPAND_MACRO(TRAP_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(__VA_ARGS__))
#else
template<typename... Args>
constexpr void TRAP_ASSERT(const Args&...) {}
#endif

#endif /*TRAP_TRAPASSERT_H*/
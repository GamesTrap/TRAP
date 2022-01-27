#ifndef TRAP_TRAPASSERT_H
#define TRAP_TRAPASSERT_H

#include <filesystem>

#if defined(TRAP_DEBUG) || defined(TRAP_RELWITHDEBINFO) || defined(ENABLE_HEADLESS_ASSERTS)
	#define TRAP_ENABLE_ASSERTS
#endif

#ifdef TRAP_ENABLE_ASSERTS
//Alternatively we could use the same "default" message for both "WITH_MSG" and "NO_MSG" and
//provide support for custom formatting by concatenating the formatting string instead of having the format inside the default message
#define TRAP_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { TP_CRITICAL(type, msg, __VA_ARGS__);\
                                                                          TRAP_DEBUG_BREAK(); } }
#define TRAP_INTERNAL_ASSERT_WITH_MSG(type, check, ...) TRAP_INTERNAL_ASSERT_IMPL\
	(\
		type, check, "Assertion '", TRAP_STRINGIFY_MACRO(check), "' failed: ", __VA_ARGS__, " @ ",\
		std::filesystem::path(__FILE__).filename(), ":", __LINE__\
	)
#define TRAP_INTERNAL_ASSERT_NO_MSG(type, check) TRAP_INTERNAL_ASSERT_IMPL\
	(\
		type, check, "Assertion '", TRAP_STRINGIFY_MACRO(check)"' failed @ ",\
		std::filesystem::path(__FILE__).filename(), ":", __LINE__\
	)

#define TRAP_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
#define TRAP_INTERNAL_ASSERT_GET_MACRO(...) TRAP_EXPAND_MACRO\
	(\
		TRAP_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, TRAP_INTERNAL_ASSERT_WITH_MSG, TRAP_INTERNAL_ASSERT_NO_MSG)\
	)

//Currently accepts at least the condition and one additional parameter (the message) being optional
#define TRAP_ASSERT(...) TRAP_EXPAND_MACRO(TRAP_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(" ", __VA_ARGS__))
#else
template<typename... Args>
constexpr void TRAP_ASSERT(const Args&...) {}
#endif

#ifdef ENABLE_HEADLESS_ASSERTS
	#define TRAP_HEADLESS_ASSERT\
	{\
			TRAP_ASSERT(TRAP::Graphics::RendererAPI::GetRenderAPI() != TRAP::Graphics::RenderAPI::Headless,\
			"Called unavailable function in Headless mode!");\
	}
#endif

#endif /*TRAP_TRAPASSERT_H*/
#pragma once

constexpr uint32_t TRAP_MAKE_VERSION(const uint32_t major, const uint32_t minor, const uint32_t patch)
{
	return major << 22 | minor << 12 | patch;
}
constexpr uint32_t TRAP_VERSION_MAJOR(const uint32_t version)
{
	return static_cast<uint32_t>(version) >> 22;
}
constexpr uint32_t TRAP_VERSION_MINOR(const uint32_t version)
{
	return static_cast<uint32_t>(version) >> 12;
}
constexpr uint32_t TRAP_VERSION_PATCH(const uint32_t version)
{
	return static_cast<uint32_t>(version) & 0xFFF;
}
constexpr uint32_t TRAP_VERSION = TRAP_MAKE_VERSION(0, 2, 75);

#ifdef TRAP_PLATFORM_WINDOWS
inline void TP_DEBUG_BREAK()
{
	__debugbreak();
}
#elif defined(TRAP_PLATFORM_LINUX)
#include <signal.h>
inline void TP_DEBUG_BREAK()
{
	raise(SIGTRAP);
}
#else
constexpr void TP_DEBUG_BREAK()
{
}
#endif

#if defined(TRAP_DEBUG) || defined(TRAP_RELWITHDEBINFO)
#define TP_ENABLE_ASSERTS
#endif

#include "Log/Log.h"

#ifdef TP_ENABLE_ASSERTS
#define TP_ASSERT(x, ...)                                                                     \
	{                                                                                         \
		if (!(x))                                                                             \
		{                                                                                     \
			TP_ERROR("Assertion Failed: ", __VA_ARGS__, " @[", __FILE__, ':', __LINE__, ']'); \
			TP_DEBUG_BREAK();                                                                 \
		}                                                                                     \
	}
#define TP_CORE_ASSERT(x, ...)                                                                       \
	{                                                                                                \
		if (!(x))                                                                                    \
		{                                                                                            \
			TP_ERROR("[Core] Assertion Failed: ", __VA_ARGS__, " @[", __FILE__, ':', __LINE__, ']'); \
			TP_DEBUG_BREAK();                                                                        \
		}                                                                                            \
	}
#define TP_RENDERER_ASSERT(x, ...)                                                                       \
	{                                                                                                    \
		if (!(x))                                                                                        \
		{                                                                                                \
			TP_ERROR("[Renderer] Assertion Failed: ", __VA_ARGS__, " @[", __FILE__, ':', __LINE__, ']'); \
			TP_DEBUG_BREAK();                                                                            \
		}                                                                                                \
	}
#else
template <typename T, typename... Args>
constexpr void TP_ASSERT(T x, const Args &... args)
{
}
template <typename T, typename... Args>
constexpr void TP_CORE_ASSERT(T x, const Args &... args)
{
}
template <typename T, typename... Args>
constexpr void TP_RENDERER_ASSERT(T x, const Args &... args)
{
}
#endif

template <typename T>
constexpr T BIT(T x)
{
	return 1 << x;
}

#define TP_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)
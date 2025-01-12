#ifndef TRAP_UTILS_ENUM_H
#define TRAP_UTILS_ENUM_H

#include "Core/Backports/ToUnderlying.h"

#ifndef MAKE_ENUM_FLAG
#define MAKE_ENUM_FLAG(ENUM_TYPE) \
	[[maybe_unused]] [[nodiscard]] constexpr ENUM_TYPE operator|(const ENUM_TYPE a, const ENUM_TYPE b) noexcept \
	{ \
		return static_cast<ENUM_TYPE>(std::to_underlying(a) | std::to_underlying(b)); \
	} \
	[[maybe_unused]] [[nodiscard]] constexpr ENUM_TYPE operator&(const ENUM_TYPE a, const ENUM_TYPE b) noexcept \
	{ \
		return static_cast<ENUM_TYPE>(std::to_underlying(a) & std::to_underlying(b)); \
	} \
	[[maybe_unused]] constexpr ENUM_TYPE operator|=(ENUM_TYPE& a, const ENUM_TYPE b) noexcept \
	{ \
	    return a = (a | b); \
	} \
	[[maybe_unused]] constexpr ENUM_TYPE operator&=(ENUM_TYPE& a, const ENUM_TYPE b) noexcept \
	{ \
	    return a = (a & b); \
	}
#endif /*MAKE_ENUM_FLAG*/

#endif /*TRAP_UTILS_ENUM_H*/

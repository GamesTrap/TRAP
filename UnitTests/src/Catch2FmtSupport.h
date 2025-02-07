#ifndef TRAP_UNITTESTS_CATCH2FMTSUPPORT_H
#define TRAP_UNITTESTS_CATCH2FMTSUPPORT_H

#include <fmt/core.h>

#include "Core/Backports/ToUnderlying.h"

namespace UnitTests::String
{
    template<typename T>
    requires (fmt::formattable<T>)
    [[nodiscard]] std::string Format(const T& t)
    {
        return fmt::format("{}", t);
    }

    template<typename T>
    requires (!fmt::formattable<T>)
    [[nodiscard]] std::string Format(const T& t)
    {
        if constexpr(std::is_base_of_v<std::exception, T>)
        {
            return t.what();
        }
        else if constexpr(std::is_enum_v<T>)
        {
            return fmt::format("{}", std::to_underlying(t));
        }
        else
        {
            return "{?}";
        }
    }
}

#ifndef CATCH_CONFIG_FALLBACK_STRINGIFIER
#define CATCH_CONFIG_FALLBACK_STRINGIFIER UnitTests::String::Format
#endif /*CATCH_CONFIG_FALLBACK_STRINGIFIER*/

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_tostring.hpp>

#endif /*TRAP_UNITTESTS_CATCH2FMTSUPPORT_H*/

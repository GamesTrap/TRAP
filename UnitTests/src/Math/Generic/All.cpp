#include <cmath>
#include <numeric>
#include <limits>
#include <concepts>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires TRAP::Math::IsVec<T> && std::same_as<typename T::value_type, bool>
    consteval void RunCompileTimeAllVecTests()
    {
        static_assert(TRAP::Math::All(T(TRAP::Math::tVec4<typename T::value_type>(true, true, true, true))));
        static_assert(TRAP::Math::All(TRAP::Math::Not(T(TRAP::Math::tVec4<typename T::value_type>(false, false, false, false)))));
    }
}

TEST_CASE("TRAP::Math::All()", "[math][generic][all]")
{
    SECTION("Vec2 - bool")
    {
        RunCompileTimeAllVecTests<TRAP::Math::Vec2b>();
    }
    SECTION("Vec3 - bool")
    {
        RunCompileTimeAllVecTests<TRAP::Math::Vec3b>();
    }
    SECTION("Vec4 - bool")
    {
        RunCompileTimeAllVecTests<TRAP::Math::Vec4b>();
    }
}

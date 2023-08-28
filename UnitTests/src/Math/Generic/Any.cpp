#include <cmath>
#include <numeric>
#include <limits>
#include <concepts>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires TRAP::Math::IsVec<T> && std::same_as<typename T::valueType, bool>
consteval void RunCompileTimeAnyVecTests()
{
    static_assert(TRAP::Math::Any(T(TRAP::Math::tVec4<typename T::valueType>(true, false, true, false))));
    static_assert(TRAP::Math::Any(TRAP::Math::Equal(T(TRAP::Math::tVec4<typename T::valueType>(true)),
                                                    T(TRAP::Math::tVec4<typename T::valueType>(true, false, true, false)))));
}

TEST_CASE("TRAP::Math::Any()", "[math][generic][any]")
{
    SECTION("Vec2 - bool")
    {
        RunCompileTimeAnyVecTests<TRAP::Math::Vec2b>();
    }
    SECTION("Vec3 - bool")
    {
        RunCompileTimeAnyVecTests<TRAP::Math::Vec3b>();
    }
    SECTION("Vec4 - bool")
    {
        RunCompileTimeAnyVecTests<TRAP::Math::Vec4b>();
    }
}
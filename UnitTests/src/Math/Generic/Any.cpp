#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires TRAP::Math::IsVec<T> && std::same_as<typename T::value_type, bool>
    consteval void RunCompileTimeAnyVecTests()
    {
        static_assert(TRAP::Math::Any(T(TRAP::Math::tVec4<typename T::value_type>(true, false, true, false))));
        static_assert(TRAP::Math::Any(TRAP::Math::Equal(T(TRAP::Math::tVec4<typename T::value_type>(true)),
                                                        T(TRAP::Math::tVec4<typename T::value_type>(true, false, true, false)))));
    }
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

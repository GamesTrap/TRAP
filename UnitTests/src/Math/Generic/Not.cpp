#include <cmath>
#include <numeric>
#include <limits>
#include <concepts>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    consteval void RunCompileTimeNotTests()
    {
        static_assert(!TRAP::Math::Not(true));
        static_assert( TRAP::Math::Not(false));
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::same_as<typename T::value_type, bool>
    consteval void RunCompileTimeNotVecTests()
    {
        constexpr T A(false);
        constexpr T B(true);
        constexpr T C(TRAP::Math::Vec4b(true, false, false, false));
        constexpr T D(TRAP::Math::Vec4b(true, true, false, false));
        constexpr T E(TRAP::Math::Vec4b(true, true, true, false));

        static_assert( TRAP::Math::All(TRAP::Math::Not(A)));
        static_assert(!TRAP::Math::All(TRAP::Math::Not(B)));
        static_assert(!TRAP::Math::All(TRAP::Math::Not(C)));
        static_assert(!TRAP::Math::All(TRAP::Math::Not(D)));
        static_assert(!TRAP::Math::All(TRAP::Math::Not(E)));
    }
}

TEST_CASE("TRAP::Math::Not()", "[math][generic][not]")
{
    SECTION("Scalar - bool")
    {
        RunCompileTimeNotTests();
    }

    SECTION("Vec2 - bool")
    {
        RunCompileTimeNotVecTests<TRAP::Math::Vec2b>();
    }
    SECTION("Vec3 - bool")
    {
        RunCompileTimeNotVecTests<TRAP::Math::Vec3b>();
    }
    SECTION("Vec4 - bool")
    {
        RunCompileTimeNotVecTests<TRAP::Math::Vec4b>();
    }
}

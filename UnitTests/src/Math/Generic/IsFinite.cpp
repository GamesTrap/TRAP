#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::IsFinite()", "[math][generic][isfinite][scalar]", f32, f64)
{
    SECTION("Normal cases")
    {
        STATIC_REQUIRE(TRAP::Math::IsFinite(TestType(1.0)));
        STATIC_REQUIRE(TRAP::Math::IsFinite(TestType(-1.0)));
    }

    SECTION("Edge cases")
    {
        static constexpr TestType min = std::numeric_limits<TestType>::min();
        static constexpr TestType max = std::numeric_limits<TestType>::max();
        static constexpr TestType inf = std::numeric_limits<TestType>::infinity();
        static constexpr TestType ninf = -std::numeric_limits<TestType>::infinity();
        static constexpr TestType nan = -std::numeric_limits<TestType>::quiet_NaN();

        STATIC_REQUIRE( TRAP::Math::IsFinite(min));
        STATIC_REQUIRE( TRAP::Math::IsFinite(max));
        STATIC_REQUIRE(!TRAP::Math::IsFinite(inf));
        STATIC_REQUIRE(!TRAP::Math::IsFinite(ninf));
        STATIC_REQUIRE(!TRAP::Math::IsFinite(nan));
    }
}

TEMPLATE_TEST_CASE("TRAP::Math::IsFinite()", "[math][generic][isfinite][vec]",
                   TRAP::Math::Vec2f, TRAP::Math::Vec2d, TRAP::Math::Vec3f, TRAP::Math::Vec3d, TRAP::Math::Vec4f, TRAP::Math::Vec4d)
{
    using Scalar = TestType::value_type;

    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::IsFinite(TestType(Scalar(1.0)))));
    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::IsFinite(TestType(Scalar(-1.0)))));
}

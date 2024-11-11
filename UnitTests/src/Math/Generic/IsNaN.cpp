#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::IsNaN()", "[math][generic][isnan][scalar]", f32, f64)
{
    static constexpr TestType nan = std::numeric_limits<TestType>::quiet_NaN();
    STATIC_REQUIRE(TRAP::Math::IsNaN(nan));
    STATIC_REQUIRE(TRAP::Math::IsNaN(-nan));
}

TEMPLATE_TEST_CASE("TRAP::Math::IsNaN()", "[math][generic][isnan][vec]",
                   TRAP::Math::Vec2f, TRAP::Math::Vec2d, TRAP::Math::Vec3f, TRAP::Math::Vec3d, TRAP::Math::Vec4f, TRAP::Math::Vec4d)
{
    using Scalar = TestType::value_type;

    static constexpr Scalar nan = std::numeric_limits<Scalar>::quiet_NaN();
    STATIC_REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(TestType(nan))));
    STATIC_REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(TestType(-nan))));
}

TEMPLATE_TEST_CASE("TRAP::Math::IsNaN()", "[math][generic][isnan][quat]",
                   TRAP::Math::Quatf, TRAP::Math::Quatd)
{
    using Scalar = TestType::value_type;

    static constexpr Scalar nan = std::numeric_limits<Scalar>::quiet_NaN();
    STATIC_REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(TestType(nan, nan, nan, nan))));
    STATIC_REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(TestType(-nan, -nan, -nan, -nan))));
}

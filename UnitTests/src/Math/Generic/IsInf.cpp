#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::IsInf()", "[math][generic][isinf][scalar]", f32, f64)
{
    static constexpr TestType inf = std::numeric_limits<TestType>::infinity();
    STATIC_REQUIRE(TRAP::Math::IsInf(inf));
    STATIC_REQUIRE(TRAP::Math::IsInf(-inf));
}

TEMPLATE_TEST_CASE("TRAP::Math::IsInf()", "[math][generic][isinf][vec]",
                   TRAP::Math::Vec2f, TRAP::Math::Vec2d, TRAP::Math::Vec3f, TRAP::Math::Vec3d, TRAP::Math::Vec4f, TRAP::Math::Vec4d)
{
    using Scalar = TestType::value_type;

    static constexpr Scalar inf = std::numeric_limits<Scalar>::infinity();
    STATIC_REQUIRE(TRAP::Math::Any(TRAP::Math::IsInf(TestType(inf))));
    STATIC_REQUIRE(TRAP::Math::Any(TRAP::Math::IsInf(TestType(-inf))));
}

TEMPLATE_TEST_CASE("TRAP::Math::IsInf()", "[math][generic][isinf][quat]",
                   TRAP::Math::Quatf, TRAP::Math::Quatd)
{
    using Scalar = TestType::value_type;

    static constexpr Scalar inf = std::numeric_limits<Scalar>::infinity();
    STATIC_REQUIRE(TRAP::Math::Any(TRAP::Math::IsInf(TestType(inf, inf, inf, inf))));
    STATIC_REQUIRE(TRAP::Math::Any(TRAP::Math::IsInf(TestType(-inf, -inf, -inf, -inf))));
}

#include <cstdint>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::XAxis()", "[math][generic][xaxis][scalar]", i8, i16, i32, i64, u8, u16, u32, u64, f32, f64)
{
    using Vec3Scalar = TRAP::Math::tVec3<TestType>;

    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::XAxis<TestType>(), Vec3Scalar(1.0f, 0.0f, 0.0f))));
}

TEMPLATE_TEST_CASE("TRAP::Math::YAxis()", "[math][generic][yaxis][scalar]", i8, i16, i32, i64, u8, u16, u32, u64, f32, f64)
{
    using Vec3Scalar = TRAP::Math::tVec3<TestType>;

    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::YAxis<TestType>(), Vec3Scalar(0.0f, 1.0f, 0.0f))));
}

TEMPLATE_TEST_CASE("TRAP::Math::ZAxis()", "[math][generic][zaxis][scalar]", i8, i16, i32, i64, u8, u16, u32, u64, f32, f64)
{
    using Vec3Scalar = TRAP::Math::tVec3<TestType>;

    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ZAxis<TestType>(), Vec3Scalar(0.0f, 0.0f, 1.0f))));
}

#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::GreaterThan()", "[math][generic][greaterthan][scalar]", i8, i16, i32, i64, u8, u16, u32, u64, f32, f64)
{
    SECTION("Normal cases")
    {
        static constexpr TestType A(TestType(1.0f));
        static constexpr TestType B(TestType(0.0f));

        STATIC_REQUIRE( TRAP::Math::GreaterThan(A, B));
        STATIC_REQUIRE(!TRAP::Math::GreaterThan(B, A));

        if constexpr(std::floating_point<TestType> || std::signed_integral<TestType>)
        {
            static constexpr TestType C(TestType(-1.0f));

            STATIC_REQUIRE(TRAP::Math::GreaterThan(B, C));
            STATIC_REQUIRE(!TRAP::Math::GreaterThan(C, B));
        }
    }

    SECTION("Edge cases")
    {
        if constexpr(std::floating_point<TestType>)
        {
            static constexpr TestType max = std::numeric_limits<TestType>::max();
            static constexpr TestType min = std::numeric_limits<TestType>::lowest();
            static constexpr TestType nan = std::numeric_limits<TestType>::quiet_NaN();
            static constexpr TestType inf = std::numeric_limits<TestType>::infinity();
            static constexpr TestType ninf = -std::numeric_limits<TestType>::infinity();

            static_assert(!TRAP::Math::GreaterThan(max, max));
            static_assert(!TRAP::Math::GreaterThan(min, min));
            static_assert( TRAP::Math::GreaterThan(max, min));
            static_assert(!TRAP::Math::GreaterThan(min, max));
            static_assert(!TRAP::Math::GreaterThan(nan, nan));
            static_assert(!TRAP::Math::GreaterThan(inf, inf));
            static_assert(!TRAP::Math::GreaterThan(ninf, ninf));
        }
    }
}

TEMPLATE_TEST_CASE("TRAP::Math::GreaterThan()", "[math][generic][greaterthan][vec]",
                   TRAP::Math::Vec2i8, TRAP::Math::Vec2i16, TRAP::Math::Vec2i32, TRAP::Math::Vec2i64,
                   TRAP::Math::Vec2ui8, TRAP::Math::Vec2ui16, TRAP::Math::Vec2ui32, TRAP::Math::Vec2ui64, TRAP::Math::Vec2f, TRAP::Math::Vec2d,
                   TRAP::Math::Vec3i8, TRAP::Math::Vec3i16, TRAP::Math::Vec3i32, TRAP::Math::Vec3i64,
                   TRAP::Math::Vec3ui8, TRAP::Math::Vec3ui16, TRAP::Math::Vec3ui32, TRAP::Math::Vec3ui64, TRAP::Math::Vec3f, TRAP::Math::Vec3d,
                   TRAP::Math::Vec4i8, TRAP::Math::Vec4i16, TRAP::Math::Vec4i32, TRAP::Math::Vec4i64,
                   TRAP::Math::Vec4ui8, TRAP::Math::Vec4ui16, TRAP::Math::Vec4ui32, TRAP::Math::Vec4ui64, TRAP::Math::Vec4f, TRAP::Math::Vec4d)
{
    using Scalar = TestType::value_type;
    using Vec4Scalar = TRAP::Math::tVec4<Scalar>;

    static constexpr TestType A(Vec4Scalar(2, 3, 4, 5));
    static constexpr TestType B(Vec4Scalar(1, 2, 3, 4));

    STATIC_REQUIRE( TRAP::Math::All(TRAP::Math::GreaterThan(A, B)));
    STATIC_REQUIRE(!TRAP::Math::All(TRAP::Math::GreaterThan(B, A)));

    if constexpr(std::floating_point<Scalar> || std::signed_integral<Scalar>)
    {
        static constexpr TestType C(Vec4Scalar(-2, -3, -4, -5));

        STATIC_REQUIRE( TRAP::Math::All(TRAP::Math::GreaterThan(A, C)));
        STATIC_REQUIRE(!TRAP::Math::All(TRAP::Math::GreaterThan(C, A)));
    }
}

TEMPLATE_TEST_CASE("TRAP::Math::GreaterThan()", "[math][generic][greaterthan][quat]", TRAP::Math::Quatd, TRAP::Math::Quatf)
{
    static constexpr TestType A(5.0f, 5.0f, 5.0f, 5.0f);
    static constexpr TestType B(0.0f, 0.0f, 0.0f, 0.0f);
    static constexpr TestType C(-1.0f, -1.0f, -1.0f, -1.0f);

    STATIC_REQUIRE( TRAP::Math::All(TRAP::Math::GreaterThan(A, B)));
    STATIC_REQUIRE(!TRAP::Math::All(TRAP::Math::GreaterThan(B, A)));
    STATIC_REQUIRE( TRAP::Math::All(TRAP::Math::GreaterThan(A, C)));
    STATIC_REQUIRE(!TRAP::Math::All(TRAP::Math::GreaterThan(C, A)));
}

#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::Equal()", "[math][generic][equal][scalar]",
                   i8, i16, i32, i64, u8, u16, u32, u64, f32, f64)
{
    SECTION("Normal cases")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        static constexpr TestType A(TestType(0.0f));
        static constexpr TestType B(TestType(5.0f));

        STATIC_REQUIRE(TRAP::Math::Equal(A, A));
        STATIC_REQUIRE(TRAP::Math::Equal(B, B));
        STATIC_REQUIRE(!TRAP::Math::Equal(A, B));
        STATIC_REQUIRE(!TRAP::Math::Equal(B, A));

        STATIC_REQUIRE(TRAP::Math::Equal(A, A, Epsilon));
        STATIC_REQUIRE(TRAP::Math::Equal(B, B, Epsilon));
        STATIC_REQUIRE(!TRAP::Math::Equal(A, B, Epsilon));
        STATIC_REQUIRE(!TRAP::Math::Equal(B, A, Epsilon));

        if constexpr(std::floating_point<TestType> || std::signed_integral<TestType>)
        {
            static constexpr TestType C(TestType(-1.0f));
            static constexpr TestType D(TestType(-5.0f));

            STATIC_REQUIRE(TRAP::Math::Equal(C, C));
            STATIC_REQUIRE(TRAP::Math::Equal(D, D));
            STATIC_REQUIRE(!TRAP::Math::Equal(C, D));
            STATIC_REQUIRE(!TRAP::Math::Equal(D, C));

            STATIC_REQUIRE(TRAP::Math::Equal(C, C, Epsilon));
            STATIC_REQUIRE(TRAP::Math::Equal(D, D, Epsilon));
            STATIC_REQUIRE(!TRAP::Math::Equal(C, D, Epsilon));
            STATIC_REQUIRE(!TRAP::Math::Equal(D, C, Epsilon));
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

            STATIC_REQUIRE(TRAP::Math::Equal(max, max));
            STATIC_REQUIRE(TRAP::Math::Equal(min, min));
            STATIC_REQUIRE(!TRAP::Math::Equal(max, min));
            STATIC_REQUIRE(!TRAP::Math::Equal(min, max));
            STATIC_REQUIRE(!TRAP::Math::Equal(nan, nan));
            STATIC_REQUIRE(TRAP::Math::Equal(inf, inf));
            STATIC_REQUIRE(TRAP::Math::Equal(ninf, ninf));
        }
    }
}

TEMPLATE_TEST_CASE("TRAP::Math::Equal()", "[math][generic][equal][vec]",
                   TRAP::Math::Vec2i8, TRAP::Math::Vec2i16, TRAP::Math::Vec2i32, TRAP::Math::Vec2i64,
                   TRAP::Math::Vec2ui8, TRAP::Math::Vec2ui16, TRAP::Math::Vec2ui32, TRAP::Math::Vec2ui64, TRAP::Math::Vec2f, TRAP::Math::Vec2d,
                   TRAP::Math::Vec3i8, TRAP::Math::Vec3i16, TRAP::Math::Vec3i32, TRAP::Math::Vec3i64,
                   TRAP::Math::Vec3ui8, TRAP::Math::Vec3ui16, TRAP::Math::Vec3ui32, TRAP::Math::Vec3ui64, TRAP::Math::Vec3f, TRAP::Math::Vec3d,
                   TRAP::Math::Vec4i8, TRAP::Math::Vec4i16, TRAP::Math::Vec4i32, TRAP::Math::Vec4i64,
                   TRAP::Math::Vec4ui8, TRAP::Math::Vec4ui16, TRAP::Math::Vec4ui32, TRAP::Math::Vec4ui64, TRAP::Math::Vec4f, TRAP::Math::Vec4d)
{
    using Scalar = TestType::value_type;

    static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

    static constexpr TestType A(TestType(0));
    static constexpr TestType B(TestType(5));

    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(A, A)));
    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(B, B)));
    STATIC_REQUIRE(!TRAP::Math::All(TRAP::Math::Equal(A, B)));
    STATIC_REQUIRE(!TRAP::Math::All(TRAP::Math::Equal(B, A)));

    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(A, A, Epsilon)));
    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(B, B, Epsilon)));
    STATIC_REQUIRE(!TRAP::Math::All(TRAP::Math::Equal(A, B, Epsilon)));
    STATIC_REQUIRE(!TRAP::Math::All(TRAP::Math::Equal(B, A, Epsilon)));

    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(A, A, TestType(Epsilon))));
    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(B, B, TestType(Epsilon))));
    STATIC_REQUIRE(!TRAP::Math::All(TRAP::Math::Equal(A, B, TestType(Epsilon))));
    STATIC_REQUIRE(!TRAP::Math::All(TRAP::Math::Equal(B, A, TestType(Epsilon))));

    if constexpr(std::floating_point<Scalar> || std::signed_integral<Scalar>)
    {
        static constexpr TestType C(TestType(-1));
        static constexpr TestType D(TestType(-5));

        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(C, C)));
        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(D, D)));
        STATIC_REQUIRE(!TRAP::Math::All(TRAP::Math::Equal(C, D)));
        STATIC_REQUIRE(!TRAP::Math::All(TRAP::Math::Equal(D, C)));

        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(C, C, Epsilon)));
        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(D, D, Epsilon)));
        STATIC_REQUIRE(!TRAP::Math::All(TRAP::Math::Equal(C, D, Epsilon)));
        STATIC_REQUIRE(!TRAP::Math::All(TRAP::Math::Equal(D, C, Epsilon)));

        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(C, C, TestType(Epsilon))));
        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(D, D, TestType(Epsilon))));
        STATIC_REQUIRE(!TRAP::Math::All(TRAP::Math::Equal(C, D, TestType(Epsilon))));
        STATIC_REQUIRE(!TRAP::Math::All(TRAP::Math::Equal(D, C, TestType(Epsilon))));
    }
}

TEMPLATE_TEST_CASE("TRAP::Math::Equal()", "[math][generic][equal][mat]",
                   TRAP::Math::Mat3d, TRAP::Math::Mat3f, TRAP::Math::Mat4d, TRAP::Math::Mat4f)
{
    using Scalar = TestType::value_type;
    using Col = TestType::col_type;

    static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

    static constexpr TestType A(0.0f);
    static constexpr TestType B(5.0f);
    static constexpr TestType C(-1.0f);
    static constexpr TestType D(-5.0f);

    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(A, A)));
    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(B, B)));
    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(C, C)));
    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(D, D)));
    STATIC_REQUIRE(!TRAP::Math::All(TRAP::Math::Equal(A, B)));
    STATIC_REQUIRE(!TRAP::Math::All(TRAP::Math::Equal(B, A)));

    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(A, A, Epsilon)));
    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(B, B, Epsilon)));
    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(C, C, Epsilon)));
    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(D, D, Epsilon)));
    STATIC_REQUIRE(!TRAP::Math::All(TRAP::Math::Equal(A, B, Epsilon)));
    STATIC_REQUIRE(!TRAP::Math::All(TRAP::Math::Equal(B, A, Epsilon)));

    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(A, A, Col(Epsilon))));
    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(B, B, Col(Epsilon))));
    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(C, C, Col(Epsilon))));
    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(D, D, Col(Epsilon))));
    STATIC_REQUIRE(!TRAP::Math::All(TRAP::Math::Equal(A, B, Col(Epsilon))));
    STATIC_REQUIRE(!TRAP::Math::All(TRAP::Math::Equal(B, A, Col(Epsilon))));
}

TEMPLATE_TEST_CASE("TRAP::Math::Equal()", "[math][generic][equal][quat]",
                   TRAP::Math::Quatd, TRAP::Math::Quatf)
{
    using Scalar = TestType::value_type;

    static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

    static constexpr TestType A(0.0f, 0.0f, 0.0f, 0.0f);
    static constexpr TestType B(5.0f, 5.0f, 5.0f, 5.0f);
    static constexpr TestType C(-1.0f, -1.0f, -1.0f, -1.0f);
    static constexpr TestType D(-5.0f, -5.0f, -5.0f, -5.0f);

    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(A, A)));
    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(B, B)));
    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(C, C)));
    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(D, D)));
    STATIC_REQUIRE(!TRAP::Math::All(TRAP::Math::Equal(A, B)));
    STATIC_REQUIRE(!TRAP::Math::All(TRAP::Math::Equal(B, A)));

    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(A, A, Epsilon)));
    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(B, B, Epsilon)));
    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(C, C, Epsilon)));
    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(D, D, Epsilon)));
    STATIC_REQUIRE(!TRAP::Math::All(TRAP::Math::Equal(A, B, Epsilon)));
    STATIC_REQUIRE(!TRAP::Math::All(TRAP::Math::Equal(B, A, Epsilon)));
}

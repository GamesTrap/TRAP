#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::Min()", "[math][generic][min][scalar]", i8, i16, i32, i64, u8, u16, u32, u64, f32, f64)
{
    SECTION("Normal cases")
    {
        static constexpr TestType N = static_cast<TestType>(0);
        static constexpr TestType B = static_cast<TestType>(1);
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Min(N, B), N, TRAP::Math::Epsilon<TestType>()));
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Min(B, N), N, TRAP::Math::Epsilon<TestType>()));

        static constexpr TestType C = static_cast<TestType>(2);
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Min(N, B, C), N, TRAP::Math::Epsilon<TestType>()));
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Min(B, N, C), N, TRAP::Math::Epsilon<TestType>()));
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Min(C, N, B), N, TRAP::Math::Epsilon<TestType>()));
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Min(C, B, N), N, TRAP::Math::Epsilon<TestType>()));
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Min(B, C, N), N, TRAP::Math::Epsilon<TestType>()));
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Min(N, C, B), N, TRAP::Math::Epsilon<TestType>()));

        static constexpr TestType D = static_cast<TestType>(3);
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Min(D, N, B, C), N, TRAP::Math::Epsilon<TestType>()));
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Min(B, D, N, C), N, TRAP::Math::Epsilon<TestType>()));
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Min(C, N, D, B), N, TRAP::Math::Epsilon<TestType>()));
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Min(C, B, D, N), N, TRAP::Math::Epsilon<TestType>()));
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Min(B, C, N, D), N, TRAP::Math::Epsilon<TestType>()));
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Min(N, C, B, D), N, TRAP::Math::Epsilon<TestType>()));
    }

    SECTION("Edge cases")
    {
        if constexpr(std::floating_point<TestType>)
        {
            static constexpr TestType NaN = std::numeric_limits<TestType>::quiet_NaN();
            static constexpr TestType B = static_cast<TestType>(1);
            REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Min(NaN, B)));
            REQUIRE(!TRAP::Math::IsNaN(TRAP::Math::Min(B, NaN)));

            static constexpr TestType C = static_cast<TestType>(2);
            REQUIRE( TRAP::Math::IsNaN(TRAP::Math::Min(NaN, B, C)));
            REQUIRE(!TRAP::Math::IsNaN(TRAP::Math::Min(B, NaN, C)));
            REQUIRE(!TRAP::Math::IsNaN(TRAP::Math::Min(C, NaN, B)));
            REQUIRE(!TRAP::Math::IsNaN(TRAP::Math::Min(C, B, NaN)));
            REQUIRE(!TRAP::Math::IsNaN(TRAP::Math::Min(B, C, NaN)));
            REQUIRE( TRAP::Math::IsNaN(TRAP::Math::Min(NaN, C, B)));

            static constexpr TestType D = static_cast<TestType>(3);
            REQUIRE(!TRAP::Math::IsNaN(TRAP::Math::Min(D, NaN, B, C)));
            REQUIRE(!TRAP::Math::IsNaN(TRAP::Math::Min(B, D, NaN, C)));
            REQUIRE(!TRAP::Math::IsNaN(TRAP::Math::Min(C, NaN, D, B)));
            REQUIRE(!TRAP::Math::IsNaN(TRAP::Math::Min(C, B, D, NaN)));
            REQUIRE(!TRAP::Math::IsNaN(TRAP::Math::Min(B, C, NaN, D)));
            REQUIRE( TRAP::Math::IsNaN(TRAP::Math::Min(NaN, C, B, D)));

        }
    }
}

TEMPLATE_TEST_CASE("TRAP::Math::Min()", "[math][generic][min][vec]",
                   TRAP::Math::Vec2i8, TRAP::Math::Vec2i16, TRAP::Math::Vec2i32, TRAP::Math::Vec2i64,
                   TRAP::Math::Vec2ui8, TRAP::Math::Vec2ui16, TRAP::Math::Vec2ui32, TRAP::Math::Vec2ui64, TRAP::Math::Vec2f, TRAP::Math::Vec2d,
                   TRAP::Math::Vec3i8, TRAP::Math::Vec3i16, TRAP::Math::Vec3i32, TRAP::Math::Vec3i64,
                   TRAP::Math::Vec3ui8, TRAP::Math::Vec3ui16, TRAP::Math::Vec3ui32, TRAP::Math::Vec3ui64, TRAP::Math::Vec3f, TRAP::Math::Vec3d,
                   TRAP::Math::Vec4i8, TRAP::Math::Vec4i16, TRAP::Math::Vec4i32, TRAP::Math::Vec4i64,
                   TRAP::Math::Vec4ui8, TRAP::Math::Vec4ui16, TRAP::Math::Vec4ui32, TRAP::Math::Vec4ui64, TRAP::Math::Vec4f, TRAP::Math::Vec4d)
{
    using Scalar = TestType::value_type;

    static constexpr TestType N = static_cast<TestType>(0);
    static constexpr TestType B = static_cast<TestType>(1);

    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Min(B, static_cast<Scalar>(N.x())), N, TRAP::Math::Epsilon<Scalar>())));

    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Min(N, B), N, TRAP::Math::Epsilon<TestType>())));
    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Min(B, N), N, TRAP::Math::Epsilon<TestType>())));

    static constexpr TestType C = static_cast<TestType>(2);
    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Min(N, B, C), N, TRAP::Math::Epsilon<TestType>())));
    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Min(B, N, C), N, TRAP::Math::Epsilon<TestType>())));
    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Min(C, N, B), N, TRAP::Math::Epsilon<TestType>())));
    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Min(C, B, N), N, TRAP::Math::Epsilon<TestType>())));
    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Min(B, C, N), N, TRAP::Math::Epsilon<TestType>())));
    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Min(N, C, B), N, TRAP::Math::Epsilon<TestType>())));

    static constexpr TestType D = static_cast<TestType>(3);
    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Min(D, N, B, C), N, TRAP::Math::Epsilon<TestType>())));
    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Min(B, D, N, C), N, TRAP::Math::Epsilon<TestType>())));
    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Min(C, N, D, B), N, TRAP::Math::Epsilon<TestType>())));
    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Min(C, B, D, N), N, TRAP::Math::Epsilon<TestType>())));
    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Min(B, C, N, D), N, TRAP::Math::Epsilon<TestType>())));
    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Min(N, C, B, D), N, TRAP::Math::Epsilon<TestType>())));
}

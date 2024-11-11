#include <cstdint>
#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::Max()", "[math][generic][max][scalar]", i8, i16, i32, i64, u8, u16, u32, u64, f32, f64)
{
    SECTION("Normal cases")
    {
        static constexpr TestType N = static_cast<TestType>(0);
        static constexpr TestType B = static_cast<TestType>(1);
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Max(N, B), B, TRAP::Math::Epsilon<TestType>()));
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Max(B, N), B, TRAP::Math::Epsilon<TestType>()));

        static constexpr TestType C = static_cast<TestType>(2);
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Max(N, B, C), C, TRAP::Math::Epsilon<TestType>()));
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Max(B, N, C), C, TRAP::Math::Epsilon<TestType>()));
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Max(C, N, B), C, TRAP::Math::Epsilon<TestType>()));
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Max(C, B, N), C, TRAP::Math::Epsilon<TestType>()));
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Max(B, C, N), C, TRAP::Math::Epsilon<TestType>()));
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Max(N, C, B), C, TRAP::Math::Epsilon<TestType>()));

        static constexpr TestType D = static_cast<TestType>(3);
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Max(D, N, B, C), D, TRAP::Math::Epsilon<TestType>()));
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Max(B, D, N, C), D, TRAP::Math::Epsilon<TestType>()));
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Max(C, N, D, B), D, TRAP::Math::Epsilon<TestType>()));
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Max(C, B, D, N), D, TRAP::Math::Epsilon<TestType>()));
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Max(B, C, N, D), D, TRAP::Math::Epsilon<TestType>()));
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Max(N, C, B, D), D, TRAP::Math::Epsilon<TestType>()));
    }

    SECTION("Edge cases")
    {
        if constexpr(std::floating_point<TestType>)
        {
            static constexpr TestType NaN = std::numeric_limits<TestType>::quiet_NaN();
            static constexpr TestType B = static_cast<TestType>(1);
            REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Max(NaN, B)));
            REQUIRE(!TRAP::Math::IsNaN(TRAP::Math::Max(B, NaN)));

            static constexpr TestType C = static_cast<TestType>(2);
            REQUIRE( TRAP::Math::IsNaN(TRAP::Math::Max(NaN, B, C)));
            REQUIRE(!TRAP::Math::IsNaN(TRAP::Math::Max(B, NaN, C)));
            REQUIRE(!TRAP::Math::IsNaN(TRAP::Math::Max(C, NaN, B)));
            REQUIRE(!TRAP::Math::IsNaN(TRAP::Math::Max(C, B, NaN)));
            REQUIRE(!TRAP::Math::IsNaN(TRAP::Math::Max(B, C, NaN)));
            REQUIRE( TRAP::Math::IsNaN(TRAP::Math::Max(NaN, C, B)));

            static constexpr TestType D = static_cast<TestType>(3);
            REQUIRE(!TRAP::Math::IsNaN(TRAP::Math::Max(D, NaN, B, C)));
            REQUIRE(!TRAP::Math::IsNaN(TRAP::Math::Max(B, D, NaN, C)));
            REQUIRE(!TRAP::Math::IsNaN(TRAP::Math::Max(C, NaN, D, B)));
            REQUIRE(!TRAP::Math::IsNaN(TRAP::Math::Max(C, B, D, NaN)));
            REQUIRE(!TRAP::Math::IsNaN(TRAP::Math::Max(B, C, NaN, D)));
            REQUIRE( TRAP::Math::IsNaN(TRAP::Math::Max(NaN, C, B, D)));
        }
    }
}

TEMPLATE_TEST_CASE("TRAP::Math::Max()", "[math][generic][max][vec]",
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

    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Max(B, static_cast<Scalar>(N.x())), B, TRAP::Math::Epsilon<Scalar>())));

    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Max(N, B), B, TRAP::Math::Epsilon<TestType>())));
    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Max(B, N), B, TRAP::Math::Epsilon<TestType>())));

    static constexpr TestType C = static_cast<TestType>(2);
    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Max(N, B, C), C, TRAP::Math::Epsilon<TestType>())));
    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Max(B, N, C), C, TRAP::Math::Epsilon<TestType>())));
    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Max(C, N, B), C, TRAP::Math::Epsilon<TestType>())));
    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Max(C, B, N), C, TRAP::Math::Epsilon<TestType>())));
    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Max(B, C, N), C, TRAP::Math::Epsilon<TestType>())));
    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Max(N, C, B), C, TRAP::Math::Epsilon<TestType>())));

    static constexpr TestType D = static_cast<TestType>(3);
    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Max(D, N, B, C), D, TRAP::Math::Epsilon<TestType>())));
    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Max(B, D, N, C), D, TRAP::Math::Epsilon<TestType>())));
    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Max(C, N, D, B), D, TRAP::Math::Epsilon<TestType>())));
    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Max(C, B, D, N), D, TRAP::Math::Epsilon<TestType>())));
    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Max(B, C, N, D), D, TRAP::Math::Epsilon<TestType>())));
    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Max(N, C, B, D), D, TRAP::Math::Epsilon<TestType>())));
}

#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::Clamp()", "[math][generic][clamp][scalar]", i8, i16, i32, i64, u8, u16, u32, u64, f32, f64)
{
    SECTION("Normal cases")
    {
        static constexpr TestType N = static_cast<TestType>(1);
        static constexpr TestType B = static_cast<TestType>(10);

        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Clamp(TestType(5), N, B), TestType(5), TRAP::Math::Epsilon<TestType>()));
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Clamp(TestType(0), N, B), TestType(1), TRAP::Math::Epsilon<TestType>()));
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Clamp(TestType(11), N, B), TestType(10), TRAP::Math::Epsilon<TestType>()));
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Clamp(TestType(1), N, B), TestType(1), TRAP::Math::Epsilon<TestType>()));
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Clamp(TestType(10), N, B), TestType(10), TRAP::Math::Epsilon<TestType>()));
    }

    SECTION("Edge cases")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        static constexpr TestType max = std::numeric_limits<TestType>::max();
        static constexpr TestType min = std::numeric_limits<TestType>::lowest();
        static constexpr TestType nan = std::numeric_limits<TestType>::quiet_NaN();

        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Clamp(max, min, max), max, Epsilon));
        if constexpr(std::floating_point<TestType>)
        {
            REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Clamp(nan, TestType(0.0), TestType(1.0))));
            STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Clamp(TestType(5.0), nan, nan), TestType(5.0), Epsilon));
        }
    }
}

TEMPLATE_TEST_CASE("TRAP::Math::Clamp()", "[math][generic][clamp][scalar]",
                   TRAP::Math::Vec2i8, TRAP::Math::Vec2i16, TRAP::Math::Vec2i32, TRAP::Math::Vec2i64,
                   TRAP::Math::Vec2ui8, TRAP::Math::Vec2ui16, TRAP::Math::Vec2ui32, TRAP::Math::Vec2ui64, TRAP::Math::Vec2f, TRAP::Math::Vec2d,
                   TRAP::Math::Vec3i8, TRAP::Math::Vec3i16, TRAP::Math::Vec3i32, TRAP::Math::Vec3i64,
                   TRAP::Math::Vec3ui8, TRAP::Math::Vec3ui16, TRAP::Math::Vec3ui32, TRAP::Math::Vec3ui64, TRAP::Math::Vec3f, TRAP::Math::Vec3d,
                   TRAP::Math::Vec4i8, TRAP::Math::Vec4i16, TRAP::Math::Vec4i32, TRAP::Math::Vec4i64,
                   TRAP::Math::Vec4ui8, TRAP::Math::Vec4ui16, TRAP::Math::Vec4ui32, TRAP::Math::Vec4ui64, TRAP::Math::Vec4f, TRAP::Math::Vec4d)
{
    using Scalar = TestType::value_type;

    SECTION("Normal cases")
    {
        static constexpr TestType N = static_cast<TestType>(1);
        static constexpr TestType B = static_cast<TestType>(10);

        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Clamp(TestType(5), N, B), TestType(5), TRAP::Math::Epsilon<TestType>())));
        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Clamp(TestType(0), N, B), TestType(1), TRAP::Math::Epsilon<TestType>())));
        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Clamp(TestType(11), N, B), TestType(10), TRAP::Math::Epsilon<TestType>())));
        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Clamp(TestType(1), N, B), TestType(1), TRAP::Math::Epsilon<TestType>())));
        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Clamp(TestType(10), N, B), TestType(10), TRAP::Math::Epsilon<TestType>())));

        static constexpr Scalar N1 = static_cast<Scalar>(1);
        static constexpr Scalar B1 = static_cast<Scalar>(10);

        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Clamp(TestType(5), N1, B1), TestType(5), TRAP::Math::Epsilon<TestType>())));
        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Clamp(TestType(0), N1, B1), TestType(1), TRAP::Math::Epsilon<TestType>())));
        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Clamp(TestType(11), N1, B1), TestType(10), TRAP::Math::Epsilon<TestType>())));
        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Clamp(TestType(1), N1, B1), TestType(1), TRAP::Math::Epsilon<TestType>())));
        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Clamp(TestType(10), N1, B1), TestType(10), TRAP::Math::Epsilon<TestType>())));
    }
}

#include <limits>
#include <cmath>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::Sin()", "[math][generic][sin][scalar]", f32, f64)
{
    SECTION("Normal cases - GCEM")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        static_assert(TRAP::Math::Equal(TRAP::Math::Sin(TestType(-1.5)), TestType(-0.997495), TestType(0.0000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Sin(TestType(0.0)), TestType(0.0), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Sin(TestType(0.001)), TestType(0.001), TestType(0.000000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Sin(TestType(1.001)), TestType(0.842011), TestType(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Sin(TestType(1.5)), TestType(0.997495), TestType(0.0000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Sin(TestType(11.1)), TestType(-0.994553), TestType(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Sin(TestType(50.0)), TestType(-0.262375), TestType(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Sin(TestType(150.0)), TestType(-0.714876), TestType(0.00001f)));
    }

    SECTION("Normal cases - std")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        static constexpr std::array<TestType, 8u> values
        {
            TestType(-1.5), TestType(0.0), TestType(0.001), TestType(1.001), TestType(1.5), TestType(11.1), TestType(50.0), TestType(150.0)
        };

        for(const TestType val : values)
            REQUIRE(TRAP::Math::Equal(TRAP::Math::Sin(val), std::sin(val), Epsilon));
    }

    SECTION("Edge cases")
    {
        static constexpr TestType nan = std::numeric_limits<TestType>::quiet_NaN();
        static constexpr TestType inf = std::numeric_limits<TestType>::infinity();
        static constexpr TestType ninf = -std::numeric_limits<TestType>::infinity();

        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Sin(nan)));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Sin(inf)));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Sin(ninf)));
    }
}

TEMPLATE_TEST_CASE("TRAP::Math::Sin()", "[math][generic][sin][vec]",
                   TRAP::Math::Vec2f, TRAP::Math::Vec2d, TRAP::Math::Vec3f, TRAP::Math::Vec3d, TRAP::Math::Vec4f, TRAP::Math::Vec4d)
{
    using Scalar = TestType::value_type;

    SECTION("Normal cases - GCEM")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Sin(TestType(Scalar(-1.5f))), TestType(-0.997495f), TestType(0.0000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Sin(TestType(Scalar(0.0f))), TestType(0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Sin(TestType(Scalar(0.001f))), TestType(0.001f), TestType(0.000000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Sin(TestType(Scalar(1.001f))), TestType(0.842011f), TestType(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Sin(TestType(Scalar(1.5f))), TestType(0.997495f), TestType(0.0000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Sin(TestType(Scalar(11.1f))), TestType(-0.994553f), TestType(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Sin(TestType(Scalar(50.0f))), TestType(-0.262375f), TestType(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Sin(TestType(Scalar(150.0f))), TestType(-0.714876f), TestType(0.00001f))));
    }

    SECTION("Normal cases - std")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        static constexpr std::array<TestType, 8u> values
        {
            TestType(Scalar(-1.5)), TestType(Scalar(0.0)), TestType(Scalar(0.001)), TestType(Scalar(1.001)),
            TestType(Scalar(1.5)), TestType(Scalar(11.1)), TestType(Scalar(50.0)), TestType(Scalar(150.0))
        };

        for(const TestType& val : values)
        {
            const TestType s = TRAP::Math::Sin(val);
            for(u32 l = 0u; l < val.Length(); ++l)
                REQUIRE(TRAP::Math::Equal(s[l], std::sin(val[l]), Epsilon));
        }
    }

    SECTION("Edge cases")
    {
        static constexpr Scalar nan = std::numeric_limits<Scalar>::quiet_NaN();
        static constexpr Scalar inf = std::numeric_limits<Scalar>::infinity();
        static constexpr Scalar ninf = -std::numeric_limits<Scalar>::infinity();

        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::Sin(TestType(nan)))));
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::Sin(TestType(inf)))));
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::Sin(TestType(ninf)))));
    }
}

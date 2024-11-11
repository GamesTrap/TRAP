#include <limits>
#include <cmath>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::ATan(x)", "[math][generic][atan][scalar]", f64, f32)
{
    SECTION("Normal cases - GCEM")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(TestType(-1.5)), TestType(-0.982794), TestType(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(TestType(-0.5)), TestType(-0.463648), TestType(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(TestType(0.0)), TestType(0.0), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(TestType(0.001)), TestType(0.001), TestType(0.000000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(TestType(0.49)), TestType(0.455616), TestType(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(TestType(0.99)), TestType(0.780373), TestType(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(TestType(1.001)), TestType(0.785898), TestType(0.0000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(TestType(1.49)), TestType(0.979703), TestType(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(TestType(1.5)), TestType(0.982794), TestType(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(TestType(1.99)), TestType(1.10514), TestType(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(TestType(2.49)), TestType(1.18891), TestType(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(TestType(2.51)), TestType(1.19166), TestType(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(TestType(3.99)), TestType(1.32523), TestType(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(TestType(7.0)), TestType(1.4289), TestType(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(TestType(11.0)), TestType(1.48014), TestType(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(TestType(11.1)), TestType(1.48095), TestType(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(TestType(25.0)), TestType(1.53082), TestType(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(TestType(50.0)), TestType(1.5508), TestType(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(TestType(101.0)), TestType(1.5609), TestType(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(TestType(150.0)), TestType(1.56413), TestType(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(TestType(900.0)), TestType(1.56969), TestType(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(TestType(1001.0)), TestType(1.5698), TestType(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(TestType(1000000.0)), TestType(1.5708), TestType(0.00001f)));
    }

    SECTION("Normal cases - std")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        static constexpr std::array<TestType, 23u> values
        {
            TestType(-1.5), TestType(-0.5), TestType(0.0), TestType(0.001), TestType(0.49), TestType(0.99), TestType(1.001), TestType(1.49), TestType(1.5),
            TestType(1.99), TestType(2.49), TestType(2.51), TestType(3.99), TestType(7.0), TestType(11.0), TestType(11.1), TestType(25.0), TestType(50.0),
            TestType(101.0), TestType(150.0), TestType(900.0), TestType(1001.0), TestType(1000000.0)
        };

        for(const TestType val : values)
            REQUIRE(TRAP::Math::Equal(TRAP::Math::ATan(val), std::atan(val), Epsilon));
    }

    SECTION("Edge cases")
    {
        static constexpr TestType nan = std::numeric_limits<TestType>::quiet_NaN();

        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::ATan(nan)));
    }
}

TEMPLATE_TEST_CASE("TRAP::Math::ATan(x)", "[math][generic][atan][vec]",
                   TRAP::Math::Vec2d, TRAP::Math::Vec2f, TRAP::Math::Vec3d, TRAP::Math::Vec3f, TRAP::Math::Vec4d, TRAP::Math::Vec4f)
{
    using Scalar = TestType::value_type;

    SECTION("Normal cases - GCEM")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(TestType(-1.5f)), TestType(-0.982794f), TestType(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(TestType(-0.5f)), TestType(-0.463648f), TestType(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(TestType(0.0f)), TestType(0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(TestType(0.001f)), TestType(0.001f), TestType(0.000000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(TestType(0.49f)), TestType(0.455616f), TestType(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(TestType(0.99f)), TestType(0.780373f), TestType(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(TestType(1.001f)), TestType(0.785898f), TestType(0.0000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(TestType(1.49f)), TestType(0.979703f), TestType(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(TestType(1.5f)), TestType(0.982794f), TestType(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(TestType(1.99f)), TestType(1.10514f), TestType(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(TestType(2.49f)), TestType(1.18891f), TestType(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(TestType(2.51f)), TestType(1.19166f), TestType(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(TestType(3.99f)), TestType(1.32523f), TestType(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(TestType(7.0f)), TestType(1.4289f), TestType(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(TestType(11.0f)), TestType(1.48014f), TestType(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(TestType(11.1f)), TestType(1.48095f), TestType(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(TestType(25.0f)), TestType(1.53082f), TestType(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(TestType(50.0f)), TestType(1.5508f), TestType(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(TestType(101.0f)), TestType(1.5609f), TestType(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(TestType(150.0f)), TestType(1.56413f), TestType(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(TestType(900.0f)), TestType(1.56969f), TestType(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(TestType(1001.0f)), TestType(1.5698f), TestType(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(TestType(1000000.0f)), TestType(1.5708f), TestType(0.00001f))));
    }

    SECTION("Normal cases - std")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        static constexpr std::array<TestType, 23u> values
        {
            TestType(Scalar(-1.5)), TestType(Scalar(-0.5)), TestType(Scalar(0.0)), TestType(Scalar(0.001)),
            TestType(Scalar(0.49)), TestType(Scalar(0.99)), TestType(Scalar(1.001)), TestType(Scalar(1.49)),
            TestType(Scalar(1.5)), TestType(Scalar(1.99)), TestType(Scalar(2.49)), TestType(Scalar(2.51)),
            TestType(Scalar(3.99)), TestType(Scalar(7.0)), TestType(Scalar(11.0)), TestType(Scalar(11.1)),
            TestType(Scalar(25.0)), TestType(Scalar(50.0)), TestType(Scalar(101.0)), TestType(Scalar(150.0)),
            TestType(Scalar(900.0)), TestType(Scalar(1001.0)), TestType(Scalar(1000000.0))
        };

        for(const TestType& val : values)
        {
            const TestType s = TRAP::Math::ATan(val);
            for(u32 l = 0u; l < val.Length(); ++l)
                REQUIRE(TRAP::Math::Equal(s[l], std::atan(val[l]), Epsilon));
        }
    }

    SECTION("Edge cases")
    {
        static constexpr Scalar nan = std::numeric_limits<Scalar>::quiet_NaN();

        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::ATan(TestType(nan)))));
    }
}

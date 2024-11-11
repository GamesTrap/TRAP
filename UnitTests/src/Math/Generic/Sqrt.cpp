#include <limits>
#include <array>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::Sqrt()", "[math][generic][sqrt][scalar]", f32, f64)
{
    SECTION("Normal cases - GCEM")
    {
        static_assert(TRAP::Math::Equal(TRAP::Math::Sqrt(TestType(0.5f)), TestType(0.707107f), TestType(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Sqrt(TestType(1.5f)), TestType(1.22474f), TestType(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Sqrt(TestType(2.0f)), TestType(1.41421f), TestType(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Sqrt(TestType(41.5f)), TestType(6.44205f), TestType(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Sqrt(TestType(0.0f)), TestType(0.0f), TestType(0.00001f)));
    }

    SECTION("Normal cases - std")
    {
        static constexpr std::array<std::tuple<TestType, TestType>, 5u> values
        {
            std::tuple(TestType(0.5f), TestType(0.707107f)),
            std::tuple(TestType(1.5f), TestType(1.22474f)),
            std::tuple(TestType(2.0f), TestType(1.41421f)),
            std::tuple(TestType(41.5f), TestType(6.44205f)),
            std::tuple(TestType(0.0f), TestType(0.0f)),
        };

        for(const auto& [x, expected] : values)
            REQUIRE(TRAP::Math::Equal(TRAP::Math::Sqrt(x), expected, TestType(0.00001f)));
    }

    SECTION("Edge cases")
    {
        static constexpr TestType nan = std::numeric_limits<TestType>::quiet_NaN();
        static constexpr TestType inf = std::numeric_limits<TestType>::infinity();
        static constexpr TestType ninf = -std::numeric_limits<TestType>::infinity();
        static constexpr TestType min = std::numeric_limits<TestType>::lowest();

        static_assert(TRAP::Math::IsNaN(TRAP::Math::Sqrt(min)));
        static_assert(TRAP::Math::IsInf(TRAP::Math::Sqrt(inf)));
        static_assert(TRAP::Math::IsNaN(TRAP::Math::Sqrt(ninf)));
        static_assert(TRAP::Math::IsNaN(TRAP::Math::Sqrt(nan)));
    }
}

TEMPLATE_TEST_CASE("TRAP::Math::Sqrt()", "[math][generic][sqrt][vec]",
                   TRAP::Math::Vec2f, TRAP::Math::Vec2d, TRAP::Math::Vec3f, TRAP::Math::Vec3d, TRAP::Math::Vec4f, TRAP::Math::Vec4d)
{
    SECTION("Normal cases - GCEM")
    {
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Sqrt(TestType(4.0f)), TestType(2.0f), TestType(0.01f))));
    }

    SECTION("Normal cases - std")
    {
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Sqrt(TestType(4.0f)), TestType(2.0f), TestType(0.01f))));
    }
}

TEMPLATE_TEST_CASE("TRAP::Math::Sqrt()", "[math][generic][sqrt][quat]", TRAP::Math::Quatf, TRAP::Math::Quatd)
{
    using Scalar = TestType::value_type;

    SECTION("Normal cases - GCEM")
    {
        static constexpr Scalar Epsilon = Scalar(0.001f);

        {
            static constexpr TestType x(Scalar(1.0f), Scalar(0.0f), Scalar(0.0f), Scalar(0.0f));
            static constexpr TestType res(Scalar(1.0f), Scalar(0.0f), Scalar(0.0f), Scalar(0.0f));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Sqrt(x), res, Epsilon)));
        }
        {
            static constexpr TestType x(Scalar(2.0f), Scalar(3.0f), Scalar(4.0f), Scalar(5.0f));
            static constexpr TestType res(Scalar(2.162f), Scalar(0.693803f), Scalar(0.92507f), Scalar(1.15634f));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Sqrt(x), res, Epsilon)));
        }
        {
            static constexpr TestType x(Scalar(-2.0f), Scalar(-3.0f), Scalar(-4.0f), Scalar(-5.0f));
            static constexpr TestType res(Scalar(1.63531f), Scalar(-0.917258f), Scalar(-1.22301f), Scalar(-1.52876f));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Sqrt(x), res, Epsilon)));
        }
    }

    SECTION("Normal cases - std")
    {
        static constexpr Scalar Epsilon = Scalar(0.001f);

        {
            static constexpr TestType x(Scalar(1.0f), Scalar(0.0f), Scalar(0.0f), Scalar(0.0f));
            static constexpr TestType res(Scalar(1.0f), Scalar(0.0f), Scalar(0.0f), Scalar(0.0f));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Sqrt(x), res, Epsilon)));
        }
        {
            static constexpr TestType x(Scalar(2.0f), Scalar(3.0f), Scalar(4.0f), Scalar(5.0f));
            static constexpr TestType res(Scalar(2.162f), Scalar(0.693803f), Scalar(0.92507f), Scalar(1.15634f));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Sqrt(x), res, Epsilon)));
        }
        {
            static constexpr TestType x(Scalar(-2.0f), Scalar(-3.0f), Scalar(-4.0f), Scalar(-5.0f));
            static constexpr TestType res(Scalar(1.63531f), Scalar(-0.917258f), Scalar(-1.22301f), Scalar(-1.52876f));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Sqrt(x), res, Epsilon)));
        }
        {
            static constexpr TestType x(Scalar(0.0f), Scalar(0.0f), Scalar(0.0f), Scalar(0.0f));
            REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::Sqrt(x))));
        }
    }
}

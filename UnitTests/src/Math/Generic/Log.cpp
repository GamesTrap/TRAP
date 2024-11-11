#include <limits>
#include <array>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::Log()", "[math][generic][log][scalar]", f32, f64)
{
    SECTION("Normal cases - GCEM")
    {
        static_assert(TRAP::Math::Equal(TRAP::Math::Log(TestType(0.5f)), TestType(-0.69314718055994529f), TestType(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Log(TestType(1.0f)), TestType(0.0f), TestType(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Log(TestType(1.5f)), TestType(0.405465f), TestType(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Log(TestType(41.5f)), TestType(3.72569346f), TestType(0.000001f)));
    }

    SECTION("Normal cases - std")
    {
        static constexpr std::array<std::tuple<TestType, TestType>, 4u> values
        {
            std::tuple(TestType(0.5f), TestType(-0.69314718055994529f)),
            std::tuple(TestType(1.0f), TestType(0.0f)),
            std::tuple(TestType(1.5f), TestType(0.405465f)),
            std::tuple(TestType(41.5f), TestType(3.72569346f))
        };

        for(const auto& [x, expected] : values)
            REQUIRE(TRAP::Math::Equal(TRAP::Math::Log(x), expected, TestType(0.000001f)));
    }

    SECTION("Edge cases")
    {
        static constexpr TestType nan = std::numeric_limits<TestType>::quiet_NaN();
        static constexpr TestType inf = std::numeric_limits<TestType>::infinity();
        static constexpr TestType ninf = -std::numeric_limits<TestType>::infinity();
        static constexpr TestType min = std::numeric_limits<TestType>::lowest();

        static_assert(TRAP::Math::IsInf(TRAP::Math::Log(TestType(0.0f))));
        static_assert(TRAP::Math::IsNaN(TRAP::Math::Log(min)));
        static_assert(TRAP::Math::IsInf(TRAP::Math::Log(inf)));
        static_assert(TRAP::Math::IsNaN(TRAP::Math::Log(ninf)));
        static_assert(TRAP::Math::IsNaN(TRAP::Math::Log(nan)));
    }
}

TEMPLATE_TEST_CASE("TRAP::Math::Log()", "[math][generic][log][vec]",
                   TRAP::Math::Vec2f, TRAP::Math::Vec2d, TRAP::Math::Vec3f, TRAP::Math::Vec3d, TRAP::Math::Vec4f, TRAP::Math::Vec4d)
{
    using Scalar = TestType::value_type;

    SECTION("Normal cases - GCEM")
    {
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Log(TestType(TRAP::Math::e<Scalar>())), TestType(1.0f), TestType(0.01f))));
    }

    SECTION("Normal cases - std")
    {
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Log(TestType(TRAP::Math::e<Scalar>())), TestType(1.0f), TestType(0.01f))));
    }
}

TEMPLATE_TEST_CASE("TRAP::Math::Log()", "[math][generic][log][quat]", TRAP::Math::Quatf, TRAP::Math::Quatd)
{
    using Scalar = TestType::value_type;
    using Vec3Scalar = TRAP::Math::tVec3<Scalar>;

    SECTION("Normal cases - GCEM")
    {
        static constexpr Scalar Epsilon = Scalar(0.001f);
        static constexpr Scalar inf = std::numeric_limits<Scalar>::infinity();

        {
            static constexpr TestType q(Vec3Scalar(1.0f, 0.0f, 0.0f), Vec3Scalar(0.0f, 1.0f, 0.0f));
            static constexpr TestType p = TRAP::Math::Log(q);
            static_assert(TRAP::Math::Any(TRAP::Math::NotEqual(q, p, Epsilon)));
        }
        {
            static constexpr TestType input(Scalar(1.0f), Scalar(0.0f), Scalar(0.0f), Scalar(0.0f));
            static constexpr TestType expected(TRAP::Math::Log(input.w()), input.x(), input.y(), input.z());
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Log(input), expected, Epsilon)));
        }
        {
            static constexpr TestType input(Scalar(-1.0f), Scalar(0.0f), Scalar(0.0f), Scalar(0.0f));
            static constexpr TestType expected(TRAP::Math::Log(-input.w()), TRAP::Math::PI<Scalar>(), input.y(), input.z());
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Log(input), expected, Epsilon)));
        }
        {
            static constexpr TestType input(Scalar(0.0f), Scalar(0.0f), Scalar(0.0f), Scalar(0.0f));
            static_assert(TRAP::Math::All(TRAP::Math::IsInf(TRAP::Math::Log(input))));
        }
    }

    SECTION("Normal cases - std")
    {
        static constexpr Scalar Epsilon = Scalar(0.001f);

        {
            const TestType q(Vec3Scalar(1.0f, 0.0f, 0.0f), Vec3Scalar(0.0f, 1.0f, 0.0f));
            const TestType p = TRAP::Math::Log(q);
            REQUIRE(TRAP::Math::Any(TRAP::Math::NotEqual(q, p, Epsilon)));
        }
        {
            const TestType input(Scalar(1.0f), Scalar(0.0f), Scalar(0.0f), Scalar(0.0f));
            const TestType expected(TRAP::Math::Log(input.w()), input.x(), input.y(), input.z());
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Log(input), expected, Epsilon)));
        }
        {
            const TestType input(Scalar(-1.0f), Scalar(0.0f), Scalar(0.0f), Scalar(0.0f));
            const TestType expected(TRAP::Math::Log(-input.w()), TRAP::Math::PI<Scalar>(), input.y(), input.z());
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Log(input), expected, Epsilon)));
        }
        {
            const TestType input(Scalar(0.0f), Scalar(0.0f), Scalar(0.0f), Scalar(0.0f));
            REQUIRE(TRAP::Math::All(TRAP::Math::IsInf(TRAP::Math::Log(input))));
        }
    }
}

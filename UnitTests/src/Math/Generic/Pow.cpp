#include <limits>
#include <array>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::Pow()", "[math][generic][pow][scalar]", f32, f64)
{
    SECTION("Normal cases - GCEM")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        static_assert(TRAP::Math::Equal(TRAP::Math::Pow(TestType(0.5f), TestType(2.0f)), TestType(0.25f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Pow(TestType(1.5f), TestType(1.0f)), TestType(1.5f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Pow(TestType(2.0f), TestType(1.0f)), TestType(2.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Pow(TestType(2.0f), TestType(2.0f)), TestType(4.0f), TestType(0.00000000000001f)));
    }

    SECTION("Normal cases - std")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        static constexpr std::array<std::tuple<TestType, TestType, TestType>, 4u> values
        {
            std::tuple(TestType(0.5f), TestType(2.0f), TestType(0.25f)),
            std::tuple(TestType(1.5f), TestType(1.0f), TestType(1.5f)),
            std::tuple(TestType(2.0f), TestType(1.0f), TestType(2.0f)),
            std::tuple(TestType(2.0f), TestType(2.0f), TestType(4.0f)),
        };

        for(const auto& [base, exponent, expected] : values)
            REQUIRE(TRAP::Math::Equal(TRAP::Math::Pow(base, exponent), expected, Epsilon));
    }

    SECTION("Edge cases")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        static constexpr TestType nan = std::numeric_limits<TestType>::quiet_NaN();
        static constexpr TestType inf = std::numeric_limits<TestType>::infinity();

        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Pow(nan, TestType(2.0f))));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Pow(TestType(2.0f), nan)));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Pow(nan, nan)));
        REQUIRE(TRAP::Math::IsInf(TRAP::Math::Pow(inf, TestType(2.0f))));
        REQUIRE(TRAP::Math::IsInf(TRAP::Math::Pow(-inf, TestType(2.0f))));
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Pow(inf, TestType(-2.0f)), TestType(0.0f), Epsilon));
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Pow(inf, TestType(0.0f)), TestType(1.0f), Epsilon));
    }
}

TEMPLATE_TEST_CASE("TRAP::Math::Pow()", "[math][generic][pow][vec]",
                   TRAP::Math::Vec2f, TRAP::Math::Vec2d, TRAP::Math::Vec3f, TRAP::Math::Vec3d, TRAP::Math::Vec4f, TRAP::Math::Vec4d)
{
    SECTION("Normal cases - GCEM")
    {
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Pow(TestType(2.0f), TestType(2.0f)), TestType(4.0f), TestType(0.00000000000001f))));
    }

    SECTION("Normal cases - std")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Pow(TestType(2.0f), TestType(2.0f)), TestType(4.0f), Epsilon)));
    }
}

TEMPLATE_TEST_CASE("TRAP::Math::Pow()", "[math][generic][pow][quat]", TRAP::Math::Quatf, TRAP::Math::Quatd)
{
    using Scalar = TestType::value_type;
    using Vec3Scalar = TRAP::Math::tVec3<Scalar>;

    SECTION("Normal cases - GCEM")
    {
        static constexpr Scalar Epsilon = Scalar(0.001f);

        static constexpr TestType q(Vec3Scalar(1.0f, 0.0f, 0.0f), Vec3Scalar(0.0f, 1.0f, 0.0f));

        {
            static constexpr Scalar one = Scalar(1.0f);
            static constexpr TestType p = TRAP::Math::Pow(q, one);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(q, p, Epsilon)));
        }
        {
            static constexpr Scalar two = Scalar(2.0f);
            static constexpr TestType p = TRAP::Math::Pow(q, two);
            static constexpr TestType r = q * q;
            static_assert(TRAP::Math::All(TRAP::Math::Equal(p, r, Epsilon)));
        }
        {
            static constexpr TestType input = TestType(Scalar(5.0f), Scalar(0.0f), Scalar(0.0f), Scalar(0.0f));
            static constexpr TestType expected = TestType(Scalar(1.0f), Scalar(0.0f), Scalar(0.0f), Scalar(0.0f));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Pow(input, Scalar(0)), expected, Epsilon)));
        }
        {
            static constexpr TestType input = TestType(Scalar(0.999f), Scalar(0.1f), Scalar(0.2f), Scalar(0.3f));
            static constexpr TestType expected = TestType(Scalar(0.858001f), Scalar(0.199800f), Scalar(0.399600f), Scalar(0.599400f));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Pow(input, Scalar(2.0f)), expected, Epsilon)));
        }
    }

    SECTION("Normal cases - std")
    {
        static constexpr Scalar Epsilon = Scalar(0.001f);

        static constexpr TestType q(Vec3Scalar(1.0f, 0.0f, 0.0f), Vec3Scalar(0.0f, 1.0f, 0.0f));

        {
            static constexpr Scalar one = Scalar(1.0f);
            const TestType p = TRAP::Math::Pow(q, one);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(q, p, Epsilon)));
        }
        {
            static constexpr Scalar two = Scalar(2.0f);
            const TestType p = TRAP::Math::Pow(q, two);
            static constexpr TestType r = q * q;
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(p, r, Epsilon)));
        }
        {
            static constexpr TestType input = TestType(Scalar(5.0f), Scalar(0.0f), Scalar(0.0f), Scalar(0.0f));
            static constexpr TestType expected = TestType(Scalar(1.0f), Scalar(0.0f), Scalar(0.0f), Scalar(0.0f));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Pow(input, Scalar(0)), expected, Epsilon)));
        }
        {
            static constexpr TestType input = TestType(Scalar(0.999f), Scalar(0.1f), Scalar(0.2f), Scalar(0.3f));
            static constexpr TestType expected = TestType(Scalar(0.858001f), Scalar(0.199800f), Scalar(0.399600f), Scalar(0.599400f));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Pow(input, Scalar(2.0f)), expected, Epsilon)));
        }
    }
}

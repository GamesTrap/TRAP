#include <cmath>
#include <limits>
#include <array>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::FMod()", "[math][generic][fmod][scalar]", f32, f64)
{
    SECTION("Normal cases - GCEM")
    {
        static_assert(TRAP::Math::Equal(TRAP::Math::FMod(TestType(  5.3f), TestType(  2.0f)), TestType( 1.3f), TestType(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::FMod(TestType(- 5.3f), TestType(  2.0f)), TestType(-1.3f), TestType(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::FMod(TestType(  5.3f), TestType(- 2.0f)), TestType( 1.3f), TestType(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::FMod(TestType(- 5.3f), TestType(- 2.0f)), TestType(-1.3f), TestType(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::FMod(TestType( 18.5f), TestType(  4.2f)), TestType( 1.7f), TestType(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::FMod(TestType(-18.5f), TestType(  4.2f)), TestType(-1.7f), TestType(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::FMod(TestType( 18.5f), TestType(- 4.2f)), TestType( 1.7f), TestType(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::FMod(TestType(-18.5f), TestType(- 4.2f)), TestType(-1.7f), TestType(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::FMod(TestType(  3.0f), TestType(  2.0f)), TestType( 1.0f), TestType(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::FMod(TestType( 22.0f), TestType(-10.0f)), TestType( 2.0f), TestType(0.00001f)));
    }

    SECTION("Normal cases - std")
    {
        static constexpr std::array<std::tuple<TestType, TestType, TestType>, 10u> values
        {
            std::tuple(TestType( 5.3f), TestType( 2.0f), TestType( 1.3f)),
            std::tuple(TestType(-5.3f), TestType( 2.0f), TestType(-1.3f)),
            std::tuple(TestType( 5.3f), TestType(-2.0f), TestType( 1.3f)),
            std::tuple(TestType(-5.3f), TestType(-2.0f), TestType(-1.3f)),
            std::tuple(TestType( 18.5f), TestType( 4.2f), TestType( 1.7f)),
            std::tuple(TestType(-18.5f), TestType( 4.2f), TestType(-1.7f)),
            std::tuple(TestType( 18.5f), TestType(-4.2f), TestType( 1.7f)),
            std::tuple(TestType(-18.5f), TestType(-4.2f), TestType(-1.7f)),
            std::tuple(TestType(  3.0f), TestType(  2.0f), TestType(1.0f)),
            std::tuple(TestType( 22.0f), TestType(-10.0f), TestType(2.0f)),
        };

        for(const auto& [x, y, expected] : values)
        {
            const TestType res = TRAP::Math::FMod(x, y);
            REQUIRE(TRAP::Math::Equal(res, expected, TestType(0.00001f)));
        }

        //http://stackoverflow.com/questions/7610631/glsl-mod-vs-hlsl-fmod
        for(TestType y = TestType(-10.0f); y < TestType(10.0f); y += TestType(0.1f))
        {
            for(TestType x = TestType(-10.0f); x < TestType(10.0f); x += TestType(0.1f))
            {
                const TestType A = std::fmod(x, y);
                // const TestType B = std::remainder(x, y);
                const TestType C = TRAP::Math::FMod(x, y);
                const TestType D = (x - y * TRAP::Math::Trunc(x / y));

                // REQUIRE(TRAP::Math::Equal(A, B, TestType(0.0001f)));
                REQUIRE(TRAP::Math::Equal(A, C, TestType(0.0001f)));
                REQUIRE(TRAP::Math::Equal(A, D, TestType(0.00001f)));
            }
        }
    }

    SECTION("Edge cases")
    {
        static constexpr TestType inf = std::numeric_limits<TestType>::infinity();
        static constexpr TestType ninf = -std::numeric_limits<TestType>::infinity();
        static constexpr TestType nan = std::numeric_limits<TestType>::quiet_NaN();

        STATIC_REQUIRE(TRAP::Math::IsNaN(TRAP::Math::FMod(inf, TestType( 0.0f))));
        STATIC_REQUIRE(TRAP::Math::IsNaN(TRAP::Math::FMod(inf, TestType(-1.0f))));
        STATIC_REQUIRE(TRAP::Math::IsNaN(TRAP::Math::FMod(inf, TestType( 1.0f))));
        STATIC_REQUIRE(TRAP::Math::IsNaN(TRAP::Math::FMod(inf, inf)));
        STATIC_REQUIRE(TRAP::Math::IsNaN(TRAP::Math::FMod(inf, ninf)));
        STATIC_REQUIRE(TRAP::Math::IsNaN(TRAP::Math::FMod(inf, nan)));

        STATIC_REQUIRE(TRAP::Math::IsNaN(TRAP::Math::FMod(ninf, TestType( 0.0f))));
        STATIC_REQUIRE(TRAP::Math::IsNaN(TRAP::Math::FMod(ninf, TestType(-1.0f))));
        STATIC_REQUIRE(TRAP::Math::IsNaN(TRAP::Math::FMod(ninf, TestType( 1.0f))));
        STATIC_REQUIRE(TRAP::Math::IsNaN(TRAP::Math::FMod(ninf, inf)));
        STATIC_REQUIRE(TRAP::Math::IsNaN(TRAP::Math::FMod(ninf, ninf)));
        STATIC_REQUIRE(TRAP::Math::IsNaN(TRAP::Math::FMod(ninf, nan)));

        STATIC_REQUIRE(TRAP::Math::IsNaN(TRAP::Math::FMod(nan, TestType( 0.0f))));
        STATIC_REQUIRE(TRAP::Math::IsNaN(TRAP::Math::FMod(nan, TestType(-1.0f))));
        STATIC_REQUIRE(TRAP::Math::IsNaN(TRAP::Math::FMod(nan, TestType( 1.0f))));
        STATIC_REQUIRE(TRAP::Math::IsNaN(TRAP::Math::FMod(nan, inf)));
        STATIC_REQUIRE(TRAP::Math::IsNaN(TRAP::Math::FMod(nan, ninf)));
        STATIC_REQUIRE(TRAP::Math::IsNaN(TRAP::Math::FMod(nan, nan)));

        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::FMod(TestType( 0.0f), TestType( 0.0f))));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::FMod(TestType(-0.0f), TestType( 0.0f))));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::FMod(TestType( 0.0f), TestType(-0.0f))));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::FMod(TestType(-0.0f), TestType(-0.0f))));
    }
}

TEMPLATE_TEST_CASE("TRAP::Math::FMod()", "[math][generic][fmod][vec]",
                   TRAP::Math::Vec2f, TRAP::Math::Vec2d, TRAP::Math::Vec3f, TRAP::Math::Vec3d, TRAP::Math::Vec4f, TRAP::Math::Vec4d)
{
    using Scalar = TestType::value_type;

    SECTION("Normal cases - GCEM")
    {
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::FMod(TestType( 5.3f ), Scalar(  2.0f)), TestType( 1.3f), Scalar(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::FMod(TestType(-5.3f ), Scalar(  2.0f)), TestType(-1.3f), Scalar(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::FMod(TestType( 5.3f ), Scalar(- 2.0f)), TestType( 1.3f), Scalar(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::FMod(TestType(-5.3f ), Scalar(- 2.0f)), TestType(-1.3f), Scalar(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::FMod(TestType( 18.5f), Scalar(  4.2f)), TestType( 1.7f), Scalar(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::FMod(TestType(-18.5f), Scalar(  4.2f)), TestType(-1.7f), Scalar(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::FMod(TestType( 18.5f), Scalar(- 4.2f)), TestType( 1.7f), Scalar(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::FMod(TestType(-18.5f), Scalar(- 4.2f)), TestType(-1.7f), Scalar(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::FMod(TestType(  3.0f), Scalar(  2.0f)), TestType( 1.0f), Scalar(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::FMod(TestType( 22.0f), Scalar(-10.0f)), TestType( 2.0f), Scalar(0.00001f))));

        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::FMod(TestType( 5.3f ), TestType(  2.0f)), TestType( 1.3f), TestType(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::FMod(TestType(-5.3f ), TestType(  2.0f)), TestType(-1.3f), TestType(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::FMod(TestType( 5.3f ), TestType(- 2.0f)), TestType( 1.3f), TestType(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::FMod(TestType(-5.3f ), TestType(- 2.0f)), TestType(-1.3f), TestType(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::FMod(TestType( 18.5f), TestType(  4.2f)), TestType( 1.7f), TestType(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::FMod(TestType(-18.5f), TestType(  4.2f)), TestType(-1.7f), TestType(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::FMod(TestType( 18.5f), TestType(- 4.2f)), TestType( 1.7f), TestType(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::FMod(TestType(-18.5f), TestType(- 4.2f)), TestType(-1.7f), TestType(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::FMod(TestType(  3.0f), TestType(  2.0f)), TestType( 1.0f), TestType(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::FMod(TestType( 22.0f), TestType(-10.0f)), TestType( 2.0f), TestType(0.00001f))));
    }

    SECTION("Normal cases - std")
    {
        static constexpr std::array<std::tuple<Scalar, Scalar, Scalar>, 10u> values
        {
            std::tuple(Scalar( 5.3f), Scalar( 2.0f), Scalar( 1.3f)),
            std::tuple(Scalar(-5.3f), Scalar( 2.0f), Scalar(-1.3f)),
            std::tuple(Scalar( 5.3f), Scalar(-2.0f), Scalar( 1.3f)),
            std::tuple(Scalar(-5.3f), Scalar(-2.0f), Scalar(-1.3f)),
            std::tuple(Scalar( 18.5f), Scalar( 4.2f), Scalar( 1.7f)),
            std::tuple(Scalar(-18.5f), Scalar( 4.2f), Scalar(-1.7f)),
            std::tuple(Scalar( 18.5f), Scalar(-4.2f), Scalar( 1.7f)),
            std::tuple(Scalar(-18.5f), Scalar(-4.2f), Scalar(-1.7f)),
            std::tuple(Scalar(  3.0f), Scalar(  2.0f), Scalar(1.0f)),
            std::tuple(Scalar( 22.0f), Scalar(-10.0f), Scalar(2.0f)),
        };

        for(const auto& [x, y, expected] : values)
        {
            const TestType res = TRAP::Math::FMod(TestType(x), y);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(res, TestType(expected), Scalar(0.00001f))));

            const TestType res1 = TRAP::Math::FMod(TestType(x), TestType(y));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(res1, TestType(expected), TestType(0.00001f))));
        }
    }
}

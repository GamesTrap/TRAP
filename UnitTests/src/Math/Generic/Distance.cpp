#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::Distance()", "[math][generic][distance][vec]",
                   TRAP::Math::Vec2d, TRAP::Math::Vec2f, TRAP::Math::Vec3d, TRAP::Math::Vec3f, TRAP::Math::Vec4d, TRAP::Math::Vec4f)
{
    using Scalar = TestType::value_type;
    using Vec4Scalar = TRAP::Math::tVec4<Scalar>;

    SECTION("Normal cases - GCEM")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        if constexpr(std::same_as<TestType, TRAP::Math::tVec2<Scalar>>)
        {
            {
                static constexpr TestType p0(1.0f, 2.0f);
                static constexpr TestType p1(5.0f, 6.0f);
                static constexpr Scalar expected(5.6568542494923806f);
                static_assert(TRAP::Math::Equal(TRAP::Math::Distance(p0, p1), expected, Scalar(0.000001f)));
            }
            {
                static constexpr TestType p0(1.0f, 2.0f);
                static constexpr TestType p1(-1.0f, -2.0f);
                static constexpr Scalar expected(4.4721359549995796f);
                static_assert(TRAP::Math::Equal(TRAP::Math::Distance(p0, p1), expected, Scalar(0.000001f)));
            }
        }
        else if constexpr(std::same_as<TestType, TRAP::Math::tVec3<Scalar>>)
        {
            {
                static constexpr TestType p0(1.0f, 2.0f, 3.0f);
                static constexpr TestType p1(5.0f, 6.0f, 7.0f);
                static constexpr Scalar expected(6.9282032302755088f);
                static_assert(TRAP::Math::Equal(TRAP::Math::Distance(p0, p1), expected, Scalar(0.000001f)));
            }
            {
                static constexpr TestType p0(1.0f, 2.0f, 3.0f);
                static constexpr TestType p1(-1.0f, -2.0f, -3.0f);
                static constexpr Scalar expected(7.48331499f);
                static_assert(TRAP::Math::Equal(TRAP::Math::Distance(p0, p1), expected, Scalar(0.000001f)));
            }
        }
        else if constexpr(std::same_as<TestType, TRAP::Math::tVec4<Scalar>>)
        {
            {
                static constexpr TestType p0(TestType(1.0f, 2.0f, 3.0f, 4.0f));
                static constexpr TestType p1(TestType(5.0f, 6.0f, 7.0f, 8.0f));
                static constexpr Scalar expected(8.0f);
                static_assert(TRAP::Math::Equal(TRAP::Math::Distance(p0, p1), expected, Epsilon));
            }
            {
                static constexpr TestType p0(TestType(1.0f, 2.0f, 3.0f, 4.0f));
                static constexpr TestType p1(TestType(-1.0f, -2.0f, -3.0f, -4.0f));
                static constexpr Scalar expected(10.954451150103322f);
                static_assert(TRAP::Math::Equal(TRAP::Math::Distance(p0, p1), expected, Scalar(0.000001f)));
            }
        }

        {
            static constexpr TestType p0(Vec4Scalar(1.0f, 0.0f, 0.0f, 0.0f));
            static constexpr Scalar expected(0.0f);
            static_assert(TRAP::Math::Equal(TRAP::Math::Distance(p0, p0), expected, Epsilon));
        }
        {
            static constexpr TestType p0(Vec4Scalar(0.0f, 0.0f, 0.0f, 0.0f));
            static constexpr Scalar expected(0.0f);
            static_assert(TRAP::Math::Equal(TRAP::Math::Distance(p0, p0), expected, Epsilon));
        }
    }

    SECTION("Normal cases - std")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        if constexpr(std::same_as<TestType, TRAP::Math::tVec2<Scalar>>)
        {
            {
                static constexpr TestType p0(1.0f, 2.0f);
                static constexpr TestType p1(5.0f, 6.0f);
                static constexpr Scalar expected(5.6568542494923806f);
                REQUIRE(TRAP::Math::Equal(TRAP::Math::Distance(p0, p1), expected, Scalar(0.000001f)));
            }
            {
                static constexpr TestType p0(1.0f, 2.0f);
                static constexpr TestType p1(-1.0f, -2.0f);
                static constexpr Scalar expected(4.4721359549995796f);
                REQUIRE(TRAP::Math::Equal(TRAP::Math::Distance(p0, p1), expected, Scalar(0.000001f)));
            }
        }
        else if constexpr(std::same_as<TestType, TRAP::Math::tVec3<Scalar>>)
        {
            {
                static constexpr TestType p0(1.0f, 2.0f, 3.0f);
                static constexpr TestType p1(5.0f, 6.0f, 7.0f);
                static constexpr Scalar expected(6.9282032302755088f);
                REQUIRE(TRAP::Math::Equal(TRAP::Math::Distance(p0, p1), expected, Scalar(0.000001f)));
            }
            {
                static constexpr TestType p0(1.0f, 2.0f, 3.0f);
                static constexpr TestType p1(-1.0f, -2.0f, -3.0f);
                static constexpr Scalar expected(7.48331499f);
                REQUIRE(TRAP::Math::Equal(TRAP::Math::Distance(p0, p1), expected, Scalar(0.000001f)));
            }
        }
        else if constexpr(std::same_as<TestType, TRAP::Math::tVec4<Scalar>>)
        {
            {
                static constexpr TestType p0(TestType(1.0f, 2.0f, 3.0f, 4.0f));
                static constexpr TestType p1(TestType(5.0f, 6.0f, 7.0f, 8.0f));
                static constexpr Scalar expected(8.0f);
                REQUIRE(TRAP::Math::Equal(TRAP::Math::Distance(p0, p1), expected, Epsilon));
            }
            {
                static constexpr TestType p0(TestType(1.0f, 2.0f, 3.0f, 4.0f));
                static constexpr TestType p1(TestType(-1.0f, -2.0f, -3.0f, -4.0f));
                static constexpr Scalar expected(10.954451150103322f);
                REQUIRE(TRAP::Math::Equal(TRAP::Math::Distance(p0, p1), expected, Scalar(0.000001f)));
            }
        }

        {
            static constexpr TestType p0(Vec4Scalar(1.0f, 0.0f, 0.0f, 0.0f));
            static constexpr Scalar expected(0.0f);
            REQUIRE(TRAP::Math::Equal(TRAP::Math::Distance(p0, p0), expected, Epsilon));
        }
        {
            static constexpr TestType p0(Vec4Scalar(0.0f, 0.0f, 0.0f, 0.0f));
            static constexpr Scalar expected(0.0f);
            REQUIRE(TRAP::Math::Equal(TRAP::Math::Distance(p0, p0), expected, Epsilon));
        }
    }

    SECTION("Edge cases")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        static constexpr Scalar max = std::numeric_limits<Scalar>::max();
        static constexpr Scalar min = std::numeric_limits<Scalar>::min();
        static constexpr Scalar nan = std::numeric_limits<Scalar>::quiet_NaN();
        static constexpr Scalar inf = std::numeric_limits<Scalar>::infinity();
        static constexpr Scalar ninf = -std::numeric_limits<Scalar>::infinity();

        {
            static constexpr TestType p0(Vec4Scalar(0.0f, 0.0f, 0.0f, 0.0f));
            static constexpr TestType p1(Vec4Scalar(max, 0.0f, 0.0f, 0.0f));
            REQUIRE(TRAP::Math::IsInf(TRAP::Math::Distance(p0, p1)));
        }
        {
            static constexpr TestType p0(Vec4Scalar(0.0f, 0.0f, 0.0f, 0.0f));
            static constexpr TestType p1(Vec4Scalar(min, 0.0f, 0.0f, 0.0f));
            static constexpr Scalar expected(0.0f);
            REQUIRE(TRAP::Math::Equal(TRAP::Math::Distance(p0, p1), expected, Epsilon));
        }
        {
            static constexpr TestType p0(Vec4Scalar(0.0f, 0.0f, 0.0f, 0.0f));
            static constexpr TestType p1(Vec4Scalar(nan, 0.0f, 0.0f, 0.0f));
            REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Distance(p0, p1)));
        }
        {
            static constexpr TestType p0(Vec4Scalar(0.0f, 0.0f, 0.0f, 0.0f));
            static constexpr TestType p1(Vec4Scalar(inf, 0.0f, 0.0f, 0.0f));
            REQUIRE(TRAP::Math::IsInf(TRAP::Math::Distance(p0, p1)));
        }
        {
            static constexpr TestType p0(Vec4Scalar(0.0f, 0.0f, 0.0f, 0.0f));
            static constexpr TestType p1(Vec4Scalar(ninf, 0.0f, 0.0f, 0.0f));
            REQUIRE(TRAP::Math::IsInf(TRAP::Math::Distance(p0, p1)));
        }
    }
}

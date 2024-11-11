#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::Inverse()", "[math][generic][inverse][mat]",
                   TRAP::Math::Mat3f, TRAP::Math::Mat3d, TRAP::Math::Mat4f, TRAP::Math::Mat4d)
{
    using Scalar = TestType::value_type;
    using Mat4Scalar = TRAP::Math::tMat4<Scalar>;

    SECTION("Normal cases")
    {
        constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        {
            static constexpr TestType a(1.0f);
            static constexpr TestType b = TRAP::Math::Inverse(a);
            static constexpr TestType i = a * b;
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(i, a, Epsilon)));
        }
        {
            static constexpr TestType a(Mat4Scalar(0.6f, 0.2f, 0.3f, 0.4f, 0.2f, 0.7f, 0.5f, 0.3f, 0.3f, 0.5f, 0.7f, 0.2f, 0.4f, 0.3f, 0.2f, 0.6f));
            static constexpr TestType b = TRAP::Math::Inverse(a);
            static constexpr TestType i = a * b;
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TestType(1.0f), i, Scalar(0.000001f))));
        }
    }

    SECTION("Edge cases")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        static constexpr Scalar min = std::numeric_limits<Scalar>::lowest();
        static constexpr Scalar max = std::numeric_limits<Scalar>::max();
        static constexpr Scalar inf = std::numeric_limits<Scalar>::infinity();
        static constexpr Scalar ninf = -std::numeric_limits<Scalar>::infinity();
        static constexpr Scalar nan = std::numeric_limits<Scalar>::quiet_NaN();

        {
            static constexpr TestType a(0.0f);
            static constexpr TestType b(Mat4Scalar{nan});
            REQUIRE(TRAP::Math::All(TRAP::Math::Not(TRAP::Math::Equal(TRAP::Math::Inverse(a), b))));
        }
        {
            static constexpr TestType a(Mat4Scalar{min});
            static constexpr TestType b(Mat4Scalar{nan});
            REQUIRE(TRAP::Math::All(TRAP::Math::Not(TRAP::Math::Equal(TRAP::Math::Inverse(a), b))));
        }
        {
            static constexpr TestType a(Mat4Scalar{max});
            static constexpr TestType b(Mat4Scalar{nan});
            REQUIRE(TRAP::Math::All(TRAP::Math::Not(TRAP::Math::Equal(TRAP::Math::Inverse(a), b))));
        }
        {
            static constexpr TestType a(Mat4Scalar{inf});
            static constexpr TestType b(Mat4Scalar{nan});
            REQUIRE(TRAP::Math::All(TRAP::Math::Not(TRAP::Math::Equal(TRAP::Math::Inverse(a), b))));
        }
        {
            static constexpr TestType a(Mat4Scalar{ninf});
            static constexpr TestType b(Mat4Scalar{nan});
            REQUIRE(TRAP::Math::All(TRAP::Math::Not(TRAP::Math::Equal(TRAP::Math::Inverse(a), b))));
        }
        {
            static constexpr TestType a(Mat4Scalar{nan});
            REQUIRE(TRAP::Math::All(TRAP::Math::Not(TRAP::Math::Equal(TRAP::Math::Inverse(a), a))));
        }
    }
}

TEMPLATE_TEST_CASE("TRAP::Math::Inverse()", "[math][generic][inverse][quat]",
                   TRAP::Math::Quatf, TRAP::Math::Quatd)
{
    using Scalar = TestType::value_type;

    SECTION("Normal cases")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        {
            static constexpr TestType q1(1.0f, 0.0f, 0.0f, 0.0f);
            static constexpr TestType res = TRAP::Math::Inverse(q1);
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(res, TestType(1.0f, 0.0f, 0.0f, 0.0f), Epsilon)));
        }
        {
            static constexpr TestType q1(0.5f, 0.5f, 0.5f, 0.5f);
            static constexpr TestType res = TRAP::Math::Inverse(q1);
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(res, TestType(0.5f, -0.5f, -0.5f, -0.5f), Epsilon)));
        }
        {
            static constexpr TestType q1(2.0f, 1.0f, 0.5f, 0.3f);
            static constexpr TestType res = TRAP::Math::Inverse(q1);
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(res, TestType(0.374532f, -0.187266f, -0.093633f, -0.056180f), Scalar(0.000001f))));
        }
        {
            static constexpr TestType q1(-0.5f, 0.2f, -0.7f, -0.9f);
            static constexpr TestType res = TRAP::Math::Inverse(q1);
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(res, TestType(-0.314465f, -0.125786f, 0.440252f, 0.566038f), Scalar(0.000001f))));
        }
    }

    SECTION("Edge cases")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        static constexpr Scalar min = std::numeric_limits<Scalar>::lowest();
        static constexpr Scalar max = std::numeric_limits<Scalar>::max();
        static constexpr Scalar inf = std::numeric_limits<Scalar>::infinity();
        static constexpr Scalar ninf = -std::numeric_limits<Scalar>::infinity();
        static constexpr Scalar nan = std::numeric_limits<Scalar>::quiet_NaN();

        {
            static constexpr TestType q1(min, min, min, min);
            const TestType res = TRAP::Math::Inverse(q1);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(res, TestType(0.0f, 0.0f, 0.0f, 0.0f), Epsilon)));
        }
        {
            static constexpr TestType q1(max, max, max, max);
            const TestType res = TRAP::Math::Inverse(q1);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(res, TestType(0.0f, 0.0f, 0.0f, 0.0f), Epsilon)));
        }
        {
            static constexpr TestType q1(inf, inf, inf, inf);
            const TestType res = TRAP::Math::Inverse(q1);
            REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(res)));
        }
        {
            static constexpr TestType q1(ninf, ninf, ninf, ninf);
            const TestType res = TRAP::Math::Inverse(q1);
            REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(res)));
        }
        {
            static constexpr TestType q1(nan, nan, nan, nan);
            const TestType res = TRAP::Math::Inverse(q1);
            REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(res)));
        }
    }
}

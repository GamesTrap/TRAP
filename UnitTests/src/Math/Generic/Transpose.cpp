#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::Transpose()", "[math][generic][transpose][mat]",
                   TRAP::Math::Mat3f, TRAP::Math::Mat3d, TRAP::Math::Mat4f, TRAP::Math::Mat4d)
{
    using Scalar = TestType::value_type;
    using Mat4Scalar = TRAP::Math::tMat4<Scalar>;

    SECTION("Normal cases")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        {
            static constexpr TestType m(Mat4Scalar(0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f));
            static constexpr TestType t = TRAP::Math::Transpose(m);
            static constexpr TestType expected(Mat4Scalar(0.0f, 4.0f, 8.0f, 12.0f, 1.0f, 5.0f, 9.0f, 13.0f, 2.0f, 6.0f, 10.0f, 14.0f, 3.0f, 7.0f, 11.0f, 15.0f));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(t, expected, Epsilon)));
        }
        {
            static constexpr TestType m(Mat4Scalar(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 2.0f, 0.0f, 0.0f, 0.0f, 0.0f, 3.0f, 0.0f, 0.0f, 0.0f, 0.0f, 4.0f));
            static constexpr TestType t = TRAP::Math::Transpose(m);
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(t, m, Epsilon)));
        }
        {
            static constexpr TestType m(Mat4Scalar(0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f));
            static constexpr TestType t = TRAP::Math::Transpose(m);
            static constexpr TestType t2 = TRAP::Math::Transpose(t);
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(m, t2, Epsilon)));
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
            static constexpr TestType m(Mat4Scalar(max, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f));
            static constexpr TestType t = TRAP::Math::Transpose(m);
            static constexpr TestType expected(Mat4Scalar(max, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(t, expected, Epsilon)));
        }
        {
            static constexpr TestType m(Mat4Scalar(min, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f));
            static constexpr TestType t = TRAP::Math::Transpose(m);
            static constexpr TestType expected(Mat4Scalar(min, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(t, expected, Epsilon)));
        }
        {
            static constexpr TestType m(Mat4Scalar(inf, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f));
            static constexpr TestType t = TRAP::Math::Transpose(m);
            static constexpr TestType expected(Mat4Scalar(inf, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(t, expected)));
        }
        {
            static constexpr TestType m(Mat4Scalar(ninf, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f));
            static constexpr TestType t = TRAP::Math::Transpose(m);
            static constexpr TestType expected(Mat4Scalar(ninf, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(t, expected)));
        }
        {
            static constexpr TestType m(Mat4Scalar(nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan));
            static constexpr TestType t = TRAP::Math::Transpose(m);
            REQUIRE(TRAP::Math::All(TRAP::Math::Not(TRAP::Math::Equal(t, m))));
        }
    }
}

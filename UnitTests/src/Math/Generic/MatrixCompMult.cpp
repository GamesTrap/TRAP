#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::MatrixCompMult()", "[math][generic][matrixcompmult][mat]",
                   TRAP::Math::Mat3f, TRAP::Math::Mat3d, TRAP::Math::Mat4f, TRAP::Math::Mat4d)
{
    using Scalar = TestType::value_type;
    using Mat4Scalar = TRAP::Math::tMat4<Scalar>;

    SECTION("Normal cases")
    {
        constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        {
            static constexpr TestType m(Mat4Scalar(0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f));
            static constexpr TestType n = TRAP::Math::MatrixCompMult(m, m);
            static constexpr TestType expected(Mat4Scalar(0.0f, 1.0f, 4.0f, 9.0f, 16.0f, 25.0f, 36.0f, 49.0f, 64.0f, 81.0f, 100.0f, 121.0f, 144.0f, 169.0f, 196.0f, 225.0f));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(n, expected, Epsilon)));
        }
        {
            static constexpr TestType m(Mat4Scalar(0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f));
            static constexpr TestType m1(Mat4Scalar(2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f));
            static constexpr TestType n = TRAP::Math::MatrixCompMult(m, m1);
            static constexpr TestType expected(Mat4Scalar(0.0f, 2.0f, 4.0f, 6.0f, 8.0f, 10.0f, 12.0f, 14.0f, 16.0f, 18.0f, 20.0f, 22.0f, 24.0f, 26.0f, 28.0f, 30.0f));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(n, expected, Epsilon)));
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
            static constexpr TestType m(Mat4Scalar{min});
            static constexpr TestType m1(Mat4Scalar(1.0f));
            static constexpr TestType n = TRAP::Math::MatrixCompMult(m, m1);
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(n, m, Epsilon)));
        }
        {
            static constexpr TestType m(Mat4Scalar{max});
            static constexpr TestType m1(Mat4Scalar(1.0f));
            static constexpr TestType n = TRAP::Math::MatrixCompMult(m, m1);
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(n, m, Epsilon)));
        }
        {
            static constexpr TestType m(Mat4Scalar{inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf});
            static constexpr TestType m1(Mat4Scalar(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f));
            static constexpr TestType n = TRAP::Math::MatrixCompMult(m, m1);
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(n, m)));
        }
        {
            static constexpr TestType m(Mat4Scalar{ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf});
            static constexpr TestType m1(Mat4Scalar(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f));
            static constexpr TestType n = TRAP::Math::MatrixCompMult(m, m1);
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(n, m)));
        }
        {
            static constexpr TestType m(Mat4Scalar{nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan});
            static constexpr TestType m1(Mat4Scalar(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f));
            const TestType n = TRAP::Math::MatrixCompMult(m, m1);
            REQUIRE(TRAP::Math::All(TRAP::Math::NotEqual(n, m)));
        }
    }
}

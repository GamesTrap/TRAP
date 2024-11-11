#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::Determinant()", "[math][generic][determinant][mat]",
                   TRAP::Math::Mat3d, TRAP::Math::Mat3f, TRAP::Math::Mat4d, TRAP::Math::Mat4f)
{
    using Scalar = TestType::value_type;
    using Mat4Scalar = TRAP::Math::tMat4<Scalar>;

    SECTION("Normal cases")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Determinant(TestType(1.0f)), Scalar(1.0f), Epsilon));
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Determinant(TestType(0.0f)), Scalar(0.0f), Epsilon));

        static constexpr TestType input1(Mat4Scalar(2.0f, 0.0f, 0.0f, 0.0f, 0.0f, 3.0f, 0.0f, 0.0f, 0.0f, 0.0f, 4.0f, 0.0f, 0.0f, 0.0f, 0.0f, 5.0f));
        static constexpr TestType input2(Mat4Scalar(2.0f, 4.0f, 1.0f, 3.0f, 1.0f, 2.0f, 4.0f, 6.0f, 3.0f, 1.0f, 2.0f, 2.0f, 5.0f, 7.0f, 3.0f, 1.0f));
        static constexpr TestType input3(Mat4Scalar(1.0f, 2.0f, 3.0f, 4.0f, 2.0f, 4.0f, 6.0f, 8.0f, 3.0f, 6.0f, 9.0f, 12.0f, 4.0f, 8.0f, 12.0f, 16.0f));
        static constexpr TestType input4(Mat4Scalar(-1.0f, 2.0f, -3.0f, 4.0f, -2.0f, 5.0f, -1.0f, 7.0f, -3.0f, 1.0f, -4.0f, 6.0f, -4.0f, 3.0f, -2.0f, 8.0f));

        if constexpr(TRAP::Math::IsMat4<TestType>)
        {
            STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Determinant(input1), Scalar(120.0f), Epsilon));
            STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Determinant(input2), Scalar(-184.0f), Epsilon));
            STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Determinant(input3), Scalar(0.0f), Epsilon));
            STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Determinant(input4), Scalar(5.0f), Epsilon));
        }
        else if constexpr(TRAP::Math::IsMat3<TestType>)
        {
            STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Determinant(input1), Scalar(24.0f), Epsilon));
            STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Determinant(input2), Scalar(35.0f), Epsilon));
            STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Determinant(input3), Scalar(0.0f), Epsilon));
            STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Determinant(input4), Scalar(-30.0f), Epsilon));
        }
    }

    SECTION("Edge cases")
    {
        static constexpr Scalar min = std::numeric_limits<Scalar>::lowest();
        static constexpr Scalar max = std::numeric_limits<Scalar>::max();
        static constexpr Scalar inf = std::numeric_limits<Scalar>::infinity();
        static constexpr Scalar ninf = -std::numeric_limits<Scalar>::infinity();
        static constexpr Scalar nan = std::numeric_limits<Scalar>::quiet_NaN();

        static constexpr TestType input1(Mat4Scalar{min});
        static constexpr TestType input2(Mat4Scalar{max});
        static constexpr TestType input3(Mat4Scalar{inf});
        static constexpr TestType input4(Mat4Scalar{ninf});
        static constexpr TestType input5(Mat4Scalar{nan});

        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Determinant(input3)));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Determinant(input4)));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Determinant(input5)));

        if constexpr(TRAP::Math::IsMat4<TestType>)
        {
            REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Determinant(input1)));
            REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Determinant(input2)));
        }
        else if constexpr(TRAP::Math::IsMat3<TestType>)
        {
            REQUIRE(TRAP::Math::IsInf(TRAP::Math::Determinant(input1)));
            REQUIRE(TRAP::Math::IsInf(TRAP::Math::Determinant(input2)));
        }
    }
}

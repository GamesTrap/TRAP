#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::OuterProduct()", "[math][generic][outerproduct][vec]",
                   TRAP::Math::Vec3f, TRAP::Math::Vec3d, TRAP::Math::Vec4f, TRAP::Math::Vec4d)
{
    using Scalar = TestType::value_type;
    using Vec4Scalar = TRAP::Math::tVec4<Scalar>;
    using Mat4Scalar = TRAP::Math::tMat4<Scalar>;

    SECTION("Normal cases")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        {
            static constexpr TestType v1(Vec4Scalar(1.0f, 2.0f, 3.0f, 4.0f));
            static constexpr TestType v2(Vec4Scalar(5.0f, 6.0f, 7.0f, 8.0f));
            static constexpr auto result = TRAP::Math::OuterProduct(v1, v2);
            static constexpr decltype(result) expected(Mat4Scalar(5.0f, 10.0f, 15.0f, 20.0f, 6.0f, 12.0f, 18.0f, 24.0f, 7.0f, 14.0f, 21.0f, 28.0f, 8.0f, 16.0f, 24.0f, 32.0f));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(result, expected, Epsilon)));
        }
        {
            static constexpr TestType v1(Vec4Scalar(1.0f, 0.0f, 0.0f, 0.0f));
            static constexpr TestType v2(Vec4Scalar(0.0f, 1.0f, 0.0f, 0.0f));
            static constexpr auto result = TRAP::Math::OuterProduct(v1, v2);
            static constexpr decltype(result) expected(Mat4Scalar(0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(result, expected, Epsilon)));
        }
        {
            static constexpr TestType v1(Vec4Scalar(2.0f));
            static constexpr TestType v2(Vec4Scalar(3.0f));
            static constexpr auto result = TRAP::Math::OuterProduct(v1, v2);
            static constexpr decltype(result) expected(Mat4Scalar(6.0f, 6.0f, 6.0f, 6.0f, 6.0f, 6.0f, 6.0f, 6.0f, 6.0f, 6.0f, 6.0f, 6.0f, 6.0f, 6.0f, 6.0f, 6.0f));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(result, expected, Epsilon)));
        }
        {
            static constexpr TestType v1(Vec4Scalar(1.0f, 0.0f, -1.0f, 0.0f));
            static constexpr TestType v2(Vec4Scalar(0.0f, 1.0f, 0.0f, -1.0f));
            static constexpr auto result = TRAP::Math::OuterProduct(v1, v2);
            static constexpr decltype(result) expected(Mat4Scalar(0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(result, expected, Epsilon)));
        }
        {
            static constexpr TestType v1(Vec4Scalar(1.0f, 2.0f, 3.0f, 4.0f));
            static constexpr TestType v2(Vec4Scalar(0.0f));
            static constexpr auto result = TRAP::Math::OuterProduct(v1, v2);
            static constexpr decltype(result) expected(Mat4Scalar(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(result, expected, Epsilon)));
        }
        {
            static constexpr TestType v1(Vec4Scalar(1.0f, 2.0f, 3.0f, 4.0f));
            static constexpr TestType v2(Vec4Scalar(1.0f, 2.0f, 3.0f, 4.0f));
            static constexpr auto result = TRAP::Math::OuterProduct(v1, v2);
            static constexpr decltype(result) expected(Mat4Scalar(1.0f, 2.0f, 3.0f, 4.0f, 2.0f, 4.0f, 6.0f, 8.0f, 3.0f, 6.0f, 9.0f, 12.0f, 4.0f, 8.0f, 12.0f, 16.0f));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(result, expected, Epsilon)));
        }
    }

    SECTION("Edge cases")
    {
        static constexpr Scalar min = std::numeric_limits<Scalar>::lowest();
        static constexpr Scalar max = std::numeric_limits<Scalar>::max();
        static constexpr Scalar inf = std::numeric_limits<Scalar>::infinity();
        static constexpr Scalar ninf = -std::numeric_limits<Scalar>::infinity();
        static constexpr Scalar nan = std::numeric_limits<Scalar>::quiet_NaN();

        {
            static constexpr TestType v1(Vec4Scalar{max});
            static constexpr TestType v2(Vec4Scalar{min});
            const auto result = TRAP::Math::OuterProduct(v1, v2);
            static constexpr decltype(result) expected(Mat4Scalar(ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(result, expected)));
        }
        {
            static constexpr TestType v(Vec4Scalar{inf});
            static constexpr auto result = TRAP::Math::OuterProduct(v, v);
            static constexpr decltype(result) expected(Mat4Scalar(inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(result, expected)));
        }
        {
            static constexpr TestType v1(Vec4Scalar{inf});
            static constexpr TestType v2(Vec4Scalar{ninf});
            static constexpr auto result = TRAP::Math::OuterProduct(v1, v2);
            static constexpr decltype(result) expected(Mat4Scalar(ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(result, expected)));
        }
        {
            static constexpr TestType v1(Vec4Scalar{nan});
            static constexpr TestType v2(Vec4Scalar{1.f});
            const auto result = TRAP::Math::OuterProduct(v1, v2);
            static constexpr decltype(result) expected(Mat4Scalar(nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan));
            REQUIRE(TRAP::Math::All(TRAP::Math::Not(TRAP::Math::Equal(result, expected))));
        }
        {
            static constexpr TestType v(Vec4Scalar{nan});
            const auto result = TRAP::Math::OuterProduct(v, v);
            static constexpr decltype(result) expected(Mat4Scalar(nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan));
            REQUIRE(TRAP::Math::All(TRAP::Math::Not(TRAP::Math::Equal(result, expected))));
        }
    }
}

#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::Translate()", "[math][generic][translate]", f32, f64)
{
    using Mat4Scalar = TRAP::Math::tMat4<TestType>;
    using Vec3Scalar = TRAP::Math::tVec3<TestType>;

    SECTION("Normal cases")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        {
            static constexpr auto t = TRAP::Math::Translate(Mat4Scalar(1.0f), Vec3Scalar(1.0f));
            static constexpr Mat4Scalar expected(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f);
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Translate(Mat4Scalar(1.0f), Vec3Scalar(1.0f)), expected, Epsilon)));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(t, TRAP::Math::Translate(Vec3Scalar(1.0f)), Epsilon)));
        }
        {
            static constexpr auto t = TRAP::Math::Translate(Mat4Scalar(1.0f), Vec3Scalar(0.0f));
            static constexpr Mat4Scalar expected(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Translate(Mat4Scalar(1.0f), Vec3Scalar(0.0f)), expected, Epsilon)));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(t, TRAP::Math::Translate(Vec3Scalar(0.0f)), Epsilon)));
        }
        {
            static constexpr auto t = TRAP::Math::Translate(Mat4Scalar(1.0f), Vec3Scalar(2.0f, 3.0f, 1.5f));
            static constexpr Mat4Scalar expected(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 2.0f, 3.0f, 1.5f, 1.0f);
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Translate(Mat4Scalar(1.0f), Vec3Scalar(2.0f, 3.0f, 1.5f)), expected, Epsilon)));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(t, TRAP::Math::Translate(Vec3Scalar(2.0f, 3.0f, 1.5f)), Epsilon)));
        }
        {
            static constexpr auto t = TRAP::Math::Translate(Mat4Scalar(1.0f), Vec3Scalar(-1.0f, -2.0f, -0.5f));
            static constexpr Mat4Scalar expected(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, -1.0f, -2.0f, -0.5f, 1.0f);
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Translate(Mat4Scalar(1.0f), Vec3Scalar(-1.0f, -2.0f, -0.5f)), expected, Epsilon)));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(t, TRAP::Math::Translate(Vec3Scalar(-1.0f, -2.0f, -0.5f)), Epsilon)));
        }
    }

    SECTION("Edge cases")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        static constexpr TestType min = std::numeric_limits<TestType>::lowest();
        static constexpr TestType max = std::numeric_limits<TestType>::max();
        static constexpr TestType inf = std::numeric_limits<TestType>::infinity();
        static constexpr TestType ninf = -std::numeric_limits<TestType>::infinity();
        static constexpr TestType nan = std::numeric_limits<TestType>::quiet_NaN();

        {
            static constexpr auto t = TRAP::Math::Translate(Mat4Scalar(1.0f), Vec3Scalar(min, 1.0f, 1.0f));
            static constexpr auto t2 = TRAP::Math::Translate(Vec3Scalar(min, 1.0f, 1.0f));
            if constexpr(std::same_as<TestType, f64>)
            {
                static constexpr Mat4Scalar expected(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, -1.7976931348623157e+308, 1.0f, 1.0f, 1.0f);
                REQUIRE(TRAP::Math::All(TRAP::Math::Equal(t, expected, Epsilon)));
            }
            else if constexpr(std::same_as<TestType, f32>)
            {
                static constexpr Mat4Scalar expected(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, -3.40282347e+38f, 1.0f, 1.0f, 1.0f);
                REQUIRE(TRAP::Math::All(TRAP::Math::Equal(t, expected, Epsilon)));
            }
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(t, t2, Epsilon)));
        }
        {
            static constexpr auto t = TRAP::Math::Translate(Mat4Scalar(1.0f), Vec3Scalar(max, 1.0f, 1.0f));
            static constexpr auto t2 = TRAP::Math::Translate(Vec3Scalar(max, 1.0f, 1.0f));
            if constexpr(std::same_as<TestType, f64>)
            {
                static constexpr Mat4Scalar expected(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.7976931348623157e+308, 1.0f, 1.0f, 1.0f);
                REQUIRE(TRAP::Math::All(TRAP::Math::Equal(t, expected, Epsilon)));
            }
            else if constexpr(std::same_as<TestType, f32>)
            {
                static constexpr Mat4Scalar expected(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 3.40282347e+38f, 1.0f, 1.0f, 1.0f);
                REQUIRE(TRAP::Math::All(TRAP::Math::Equal(t, expected, Epsilon)));
            }
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(t, t2, Epsilon)));
        }
        {
            const auto t = TRAP::Math::Translate(Mat4Scalar(1.0f), Vec3Scalar(inf, 1.0f, 1.0f));
            const auto t2 = TRAP::Math::Translate(Vec3Scalar(inf, 1.0f, 1.0f));
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<3u>(t))));
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<3u>(t2))));
        }
        {
            const auto t = TRAP::Math::Translate(Mat4Scalar(1.0f), Vec3Scalar(ninf, 1.0f, 1.0f));
            const auto t2 = TRAP::Math::Translate(Vec3Scalar(ninf, 1.0f, 1.0f));
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<3u>(t))));
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<3u>(t2))));
        }
        {
            const auto t = TRAP::Math::Translate(Mat4Scalar(1.0f), Vec3Scalar(nan, 1.0f, 1.0f));
            const auto t2 = TRAP::Math::Translate(Vec3Scalar(nan, 1.0f, 1.0f));
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<3u>(t))));
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<3u>(t2))));
        }
    }
}

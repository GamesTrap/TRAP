#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::Scale()", "[math][generic][scale][scalar]", f32, f64)
{
    using Vec3Scalar = TRAP::Math::tVec3<TestType>;
    using Mat4Scalar = TRAP::Math::tMat4<TestType>;

    SECTION("Normal cases")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        {
            static constexpr Vec3Scalar val(2.0f);
            static constexpr auto res = TRAP::Math::Scale(Mat4Scalar(1.0f), val);
            static constexpr Mat4Scalar expected = Mat4Scalar(2.0f, 0.0f, 0.0f, 0.0f, 0.0f, 2.0f, 0.0f, 0.0f, 0.0f, 0.0f, 2.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Scale(Mat4Scalar(1.0f), val), expected, Epsilon)));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Scale(val), res, Epsilon)));
        }
        {
            static constexpr Vec3Scalar val(2.0f, 3.0f, 4.0f);
            static constexpr Mat4Scalar res = TRAP::Math::Scale(Mat4Scalar(1.0f), val);
            static constexpr Mat4Scalar expected = Mat4Scalar(2.0f, 0.0f, 0.0f, 0.0f, 0.0f, 3.0f, 0.0f, 0.0f, 0.0f, 0.0f, 4.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Scale(Mat4Scalar(1.0f), val), expected, Epsilon)));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Scale(val), res, Epsilon)));
        }
        {
            static constexpr Vec3Scalar val(0.5f);
            static constexpr Mat4Scalar res = TRAP::Math::Scale(Mat4Scalar(1.0f), val);
            static constexpr Mat4Scalar expected = Mat4Scalar(0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Scale(Mat4Scalar(1.0f), val), expected, Epsilon)));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Scale(val), res, Epsilon)));
        }
        {
            static constexpr Vec3Scalar val(1.0f);
            static constexpr Mat4Scalar res = TRAP::Math::Scale(Mat4Scalar(1.0f), val);
            static constexpr Mat4Scalar expected = Mat4Scalar(1.0f);
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Scale(Mat4Scalar(1.0f), val), expected, Epsilon)));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Scale(val), res, Epsilon)));
        }
        {
            static constexpr Vec3Scalar val(0.0f);
            static constexpr Mat4Scalar res = TRAP::Math::Scale(Mat4Scalar(1.0f), val);
            static constexpr Mat4Scalar expected = Mat4Scalar(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Scale(Mat4Scalar(1.0f), val), expected, Epsilon)));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Scale(val), res, Epsilon)));
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
            static constexpr Vec3Scalar val(min);
            static constexpr Mat4Scalar res = TRAP::Math::Scale(Mat4Scalar(1.0f), val);
            static constexpr Mat4Scalar expected = Mat4Scalar(min, 0.0f, 0.0f, 0.0f, 0.0f, min, 0.0f, 0.0f, 0.0f, 0.0f, min, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(res, expected, Epsilon)));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Scale(val), res, Epsilon)));
        }
        {
            static constexpr Vec3Scalar val(max);
            static constexpr Mat4Scalar res = TRAP::Math::Scale(Mat4Scalar(1.0f), val);
            static constexpr Mat4Scalar expected = Mat4Scalar(max, 0.0f, 0.0f, 0.0f, 0.0f, max, 0.0f, 0.0f, 0.0f, 0.0f, max, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(res, expected, Epsilon)));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Scale(val), res, Epsilon)));
        }
        {
            static constexpr Vec3Scalar val(inf);
            const Mat4Scalar res = TRAP::Math::Scale(Mat4Scalar(1.0f), val);
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<0u>(res))));
        }
        {
            static constexpr Vec3Scalar val(ninf);
            const Mat4Scalar res = TRAP::Math::Scale(Mat4Scalar(1.0f), val);
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<0u>(res))));
        }
        {
            static constexpr Vec3Scalar val(nan);
            const Mat4Scalar res = TRAP::Math::Scale(Mat4Scalar(1.0f), val);
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<0u>(res))));
        }
    }
}

#include <limits>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires std::floating_point<T>
    consteval void RunCompileTimeScaleTests()
    {
        constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        {
            constexpr TRAP::Math::tVec3<T> val(2.0f);
            constexpr TRAP::Math::tMat4<T> res = TRAP::Math::Scale(TRAP::Math::tMat4<T>(1.0f), val);
            constexpr TRAP::Math::tMat4<T> expected = TRAP::Math::tMat4<T>(2.0f, 0.0f, 0.0f, 0.0f, 0.0f, 2.0f, 0.0f, 0.0f, 0.0f, 0.0f, 2.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(res, expected, Epsilon)));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Scale(val), res, Epsilon)));
        }
        {
            constexpr TRAP::Math::tVec3<T> val(2.0f, 3.0f, 4.0f);
            constexpr TRAP::Math::tMat4<T> res = TRAP::Math::Scale(TRAP::Math::tMat4<T>(1.0f), val);
            constexpr TRAP::Math::tMat4<T> expected = TRAP::Math::tMat4<T>(2.0f, 0.0f, 0.0f, 0.0f, 0.0f, 3.0f, 0.0f, 0.0f, 0.0f, 0.0f, 4.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(res, expected, Epsilon)));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Scale(val), res, Epsilon)));
        }
        {
            constexpr TRAP::Math::tVec3<T> val(0.5f);
            constexpr TRAP::Math::tMat4<T> res = TRAP::Math::Scale(TRAP::Math::tMat4<T>(1.0f), val);
            constexpr TRAP::Math::tMat4<T> expected = TRAP::Math::tMat4<T>(0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(res, expected, Epsilon)));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Scale(val), res, Epsilon)));
        }
        {
            constexpr TRAP::Math::tVec3<T> val(1.0f);
            constexpr TRAP::Math::tMat4<T> res = TRAP::Math::Scale(TRAP::Math::tMat4<T>(1.0f), val);
            constexpr TRAP::Math::tMat4<T> expected = TRAP::Math::tMat4<T>(1.0f);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(res, expected, Epsilon)));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Scale(val), res, Epsilon)));
        }
        {
            constexpr TRAP::Math::tVec3<T> val(0.0f);
            constexpr TRAP::Math::tMat4<T> res = TRAP::Math::Scale(TRAP::Math::tMat4<T>(1.0f), val);
            constexpr TRAP::Math::tMat4<T> expected = TRAP::Math::tMat4<T>(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(res, expected, Epsilon)));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Scale(val), res, Epsilon)));
        }
    }

    template<typename T>
    requires std::floating_point<T>
    void RunScaleEdgeTests()
    {
        static constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        static constexpr T min = std::numeric_limits<T>::lowest();
        static constexpr T max = std::numeric_limits<T>::max();
        static constexpr T inf = std::numeric_limits<T>::infinity();
        static constexpr T ninf = -std::numeric_limits<T>::infinity();
        static constexpr T nan = std::numeric_limits<T>::quiet_NaN();

        {
            static constexpr TRAP::Math::tVec3<T> val(min);
            static constexpr TRAP::Math::tMat4<T> res = TRAP::Math::Scale(TRAP::Math::tMat4<T>(1.0f), val);
            static constexpr TRAP::Math::tMat4<T> expected = TRAP::Math::tMat4<T>(min, 0.0f, 0.0f, 0.0f, 0.0f, min, 0.0f, 0.0f, 0.0f, 0.0f, min, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(res, expected, Epsilon)));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Scale(val), res, Epsilon)));
        }
        {
            static constexpr TRAP::Math::tVec3<T> val(max);
            static constexpr TRAP::Math::tMat4<T> res = TRAP::Math::Scale(TRAP::Math::tMat4<T>(1.0f), val);
            static constexpr TRAP::Math::tMat4<T> expected = TRAP::Math::tMat4<T>(max, 0.0f, 0.0f, 0.0f, 0.0f, max, 0.0f, 0.0f, 0.0f, 0.0f, max, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(res, expected, Epsilon)));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Scale(val), res, Epsilon)));
        }
        {
            static constexpr TRAP::Math::tVec3<T> val(inf);
            const TRAP::Math::tMat4<T> res = TRAP::Math::Scale(TRAP::Math::tMat4<T>(1.0f), val);
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<0>(res))));
        }
        {
            static constexpr TRAP::Math::tVec3<T> val(ninf);
            const TRAP::Math::tMat4<T> res = TRAP::Math::Scale(TRAP::Math::tMat4<T>(1.0f), val);
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<0>(res))));
        }
        {
            static constexpr TRAP::Math::tVec3<T> val(nan);
            const TRAP::Math::tMat4<T> res = TRAP::Math::Scale(TRAP::Math::tMat4<T>(1.0f), val);
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<0>(res))));
        }
    }
}

TEST_CASE("TRAP::Math::Scale()", "[math][generic][scale]")
{
    SECTION("Scalar - f64")
    {
        RunCompileTimeScaleTests<f64>();
        RunScaleEdgeTests<f64>();
    }
    SECTION("Scalar - f32")
    {
        RunCompileTimeScaleTests<f32>();
        RunScaleEdgeTests<f32>();
    }
}

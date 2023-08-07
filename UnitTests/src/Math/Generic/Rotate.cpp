#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>
#include <array>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::floating_point<T>
void RunRotateTests()
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    {
        const auto t = TRAP::Math::Rotate(TRAP::Math::tMat4<T>(1.0f), TRAP::Math::Radians<T>(90.0f), TRAP::Math::tVec3<T>(1.0f));
        const auto t2 = TRAP::Math::Rotate(TRAP::Math::Radians<T>(90.0f), TRAP::Math::tVec3<T>(1.0f));
        constexpr TRAP::Math::tMat4<T> expected(0.333333f, 0.910684f, -0.244017f, 0.0f, -0.244017f, 0.333333f, 0.910684f, 0.0f, 0.910684f, -0.244017f, 0.333333f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(t, expected, T(0.000001f))));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(t, t2, Epsilon)));
    }
    {
        const auto t = TRAP::Math::Rotate(TRAP::Math::tMat4<T>(1.0f), TRAP::Math::Radians<T>(90.0f), TRAP::Math::tVec3<T>(1.0f, 0.0f, 0.0f));
        const auto t2 = TRAP::Math::Rotate(TRAP::Math::Radians<T>(90.0f), TRAP::Math::tVec3<T>(1.0f, 0.0f, 0.0f));
        constexpr TRAP::Math::tMat4<T> expected(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(t, expected, T(0.000001f))));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(t, t2, Epsilon)));
    }
    {
        const auto t = TRAP::Math::Rotate(TRAP::Math::tMat4<T>(1.0f), TRAP::Math::Radians<T>(45.0f), TRAP::Math::tVec3<T>(0.0f, 1.0f, 0.0f));
        const auto t2 = TRAP::Math::Rotate(TRAP::Math::Radians<T>(45.0f), TRAP::Math::tVec3<T>(0.0f, 1.0f, 0.0f));
        constexpr TRAP::Math::tMat4<T> expected(0.707107f, 0.0f, -0.707107f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.707107f, 0.0f, 0.707107f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(t, expected, T(0.000001f))));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(t, t2, Epsilon)));
    }
    {
        const auto t = TRAP::Math::Rotate(TRAP::Math::tMat4<T>(1.0f), TRAP::Math::Radians<T>(60.0f), TRAP::Math::tVec3<T>(0.0f, 0.0f, 1.0f));
        const auto t2 = TRAP::Math::Rotate(TRAP::Math::Radians<T>(60.0f), TRAP::Math::tVec3<T>(0.0f, 0.0f, 1.0f));
        constexpr TRAP::Math::tMat4<T> expected(0.5f, 0.866025f, 0.0f, 0.0f, -0.866025f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(t, expected, T(0.000001f))));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(t, t2, Epsilon)));
    }
    {
        const auto t = TRAP::Math::Rotate(TRAP::Math::tMat4<T>(1.0f), TRAP::Math::Radians<T>(60.0f), TRAP::Math::tVec3<T>(0.5f, 0.5f, 0.5f));
        const auto t2 = TRAP::Math::Rotate(TRAP::Math::Radians<T>(60.0f), TRAP::Math::tVec3<T>(0.5f, 0.5f, 0.5f));
        constexpr TRAP::Math::tMat4<T> expected(0.666667f, 0.666667f, -0.333333f, 0.0f, -0.333333f, 0.666667f, 0.666667f, 0.0f, 0.666667f, -0.333333f, 0.666667f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(t, expected, T(0.000001f))));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(t, t2, Epsilon)));
    }
}

template<typename T>
requires std::floating_point<T>
void RunRotateEdgeTests()
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    constexpr T min = std::numeric_limits<T>::lowest();
    constexpr T max = std::numeric_limits<T>::max();
    constexpr T inf = std::numeric_limits<T>::infinity();
    constexpr T ninf = -std::numeric_limits<T>::infinity();
    constexpr T nan = std::numeric_limits<T>::quiet_NaN();

    {
        const auto t = TRAP::Math::Rotate(TRAP::Math::tMat4<T>(1.0f), TRAP::Math::Radians<T>(90.0f), TRAP::Math::tVec3<T>(min, 1.0f, 1.0f));
        const auto t2 = TRAP::Math::Rotate(TRAP::Math::Radians<T>(90.0f), TRAP::Math::tVec3<T>(min, 1.0f, 1.0f));
        constexpr TRAP::Math::tMat4<T> expected(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(t, expected, Epsilon)));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(t, t2, Epsilon)));
    }
    {
        const auto t = TRAP::Math::Rotate(TRAP::Math::tMat4<T>(1.0f), TRAP::Math::Radians<T>(90.0f), TRAP::Math::tVec3<T>(max, 1.0f, 1.0f));
        const auto t2 = TRAP::Math::Rotate(TRAP::Math::Radians<T>(90.0f), TRAP::Math::tVec3<T>(max, 1.0f, 1.0f));
        constexpr TRAP::Math::tMat4<T> expected(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(t, expected, Epsilon)));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(t, t2, Epsilon)));
    }
    {
        const auto t = TRAP::Math::Rotate(TRAP::Math::tMat4<T>(1.0f), TRAP::Math::Radians<T>(90.0f), TRAP::Math::tVec3<T>(inf, 1.0f, 1.0f));
        const auto t2 = TRAP::Math::Rotate(TRAP::Math::Radians<T>(90.0f), TRAP::Math::tVec3<T>(inf, 1.0f, 1.0f));
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(t[0])));
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(t2[0])));
    }
    {
        const auto t = TRAP::Math::Rotate(TRAP::Math::tMat4<T>(1.0f), TRAP::Math::Radians<T>(90.0f), TRAP::Math::tVec3<T>(ninf, 1.0f, 1.0f));
        const auto t2 = TRAP::Math::Rotate(TRAP::Math::Radians<T>(90.0f), TRAP::Math::tVec3<T>(ninf, 1.0f, 1.0f));
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(t[0])));
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(t2[0])));
    }
    {
        const auto t = TRAP::Math::Rotate(TRAP::Math::tMat4<T>(1.0f), TRAP::Math::Radians<T>(90.0f), TRAP::Math::tVec3<T>(nan, 1.0f, 1.0f));
        const auto t2 = TRAP::Math::Rotate(TRAP::Math::Radians<T>(90.0f), TRAP::Math::tVec3<T>(nan, 1.0f, 1.0f));
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(t[0])));
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(t2[0])));
    }
}

TEST_CASE("TRAP::Math::Rotate()", "[math][generic][rotate]")
{
    SECTION("Scalar - double")
    {
        RunRotateTests<double>();
        RunRotateEdgeTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunRotateTests<float>();
        RunRotateEdgeTests<float>();
    }
}

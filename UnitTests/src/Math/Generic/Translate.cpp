#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>
#include <array>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::floating_point<T>
consteval void RunCompileTimeTranslateTests()
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    {
        constexpr auto t = TRAP::Math::Translate(TRAP::Math::tMat4<T>(1.0f), TRAP::Math::tVec3<T>(1.0f));
        constexpr auto t2 = TRAP::Math::Translate(TRAP::Math::tVec3<T>(1.0f));
        constexpr TRAP::Math::tMat4<T> expected(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f);
        static_assert(TRAP::Math::All(TRAP::Math::Equal(t, expected, Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(t, t2, Epsilon)));
    }
    {
        constexpr auto t = TRAP::Math::Translate(TRAP::Math::tMat4<T>(1.0f), TRAP::Math::tVec3<T>(0.0f));
        constexpr auto t2 = TRAP::Math::Translate(TRAP::Math::tVec3<T>(0.0f));
        constexpr TRAP::Math::tMat4<T> expected(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
        static_assert(TRAP::Math::All(TRAP::Math::Equal(t, expected, Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(t, t2, Epsilon)));
    }
    {
        constexpr auto t = TRAP::Math::Translate(TRAP::Math::tMat4<T>(1.0f), TRAP::Math::tVec3<T>(2.0f, 3.0f, 1.5f));
        constexpr auto t2 = TRAP::Math::Translate(TRAP::Math::tVec3<T>(2.0f, 3.0f, 1.5f));
        constexpr TRAP::Math::tMat4<T> expected(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 2.0f, 3.0f, 1.5f, 1.0f);
        static_assert(TRAP::Math::All(TRAP::Math::Equal(t, expected, Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(t, t2, Epsilon)));
    }
    {
        constexpr auto t = TRAP::Math::Translate(TRAP::Math::tMat4<T>(1.0f), TRAP::Math::tVec3<T>(-1.0f, -2.0f, -0.5f));
        constexpr auto t2 = TRAP::Math::Translate(TRAP::Math::tVec3<T>(-1.0f, -2.0f, -0.5f));
        constexpr TRAP::Math::tMat4<T> expected(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, -1.0f, -2.0f, -0.5f, 1.0f);
        static_assert(TRAP::Math::All(TRAP::Math::Equal(t, expected, Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(t, t2, Epsilon)));
    }
}

template<typename T>
requires std::floating_point<T>
void RunTranslateEdgeTests()
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    constexpr T min = std::numeric_limits<T>::lowest();
    constexpr T max = std::numeric_limits<T>::max();
    constexpr T inf = std::numeric_limits<T>::infinity();
    constexpr T ninf = -std::numeric_limits<T>::infinity();
    constexpr T nan = std::numeric_limits<T>::quiet_NaN();

    {
        constexpr auto t = TRAP::Math::Translate(TRAP::Math::tMat4<T>(1.0f), TRAP::Math::tVec3<T>(min, 1.0f, 1.0f));
        constexpr auto t2 = TRAP::Math::Translate(TRAP::Math::tVec3<T>(min, 1.0f, 1.0f));
        if constexpr(std::same_as<T, double>)
        {
            constexpr TRAP::Math::tMat4<T> expected(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, -1.7976931348623157e+308, 1.0f, 1.0f, 1.0f);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(t, expected, Epsilon)));
        }
        else if constexpr(std::same_as<T, float>)
        {
            constexpr TRAP::Math::tMat4<T> expected(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, -3.40282347e+38f, 1.0f, 1.0f, 1.0f);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(t, expected, Epsilon)));
        }
        static_assert(TRAP::Math::All(TRAP::Math::Equal(t, t2, Epsilon)));
    }
    {
        constexpr auto t = TRAP::Math::Translate(TRAP::Math::tMat4<T>(1.0f), TRAP::Math::tVec3<T>(max, 1.0f, 1.0f));
        constexpr auto t2 = TRAP::Math::Translate(TRAP::Math::tVec3<T>(max, 1.0f, 1.0f));
        if constexpr(std::same_as<T, double>)
        {
            constexpr TRAP::Math::tMat4<T> expected(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.7976931348623157e+308, 1.0f, 1.0f, 1.0f);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(t, expected, Epsilon)));
        }
        else if constexpr(std::same_as<T, float>)
        {
            constexpr TRAP::Math::tMat4<T> expected(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 3.40282347e+38f, 1.0f, 1.0f, 1.0f);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(t, expected, Epsilon)));
        }
        static_assert(TRAP::Math::All(TRAP::Math::Equal(t, t2, Epsilon)));
    }
    {
        const auto t = TRAP::Math::Translate(TRAP::Math::tMat4<T>(1.0f), TRAP::Math::tVec3<T>(inf, 1.0f, 1.0f));
        const auto t2 = TRAP::Math::Translate(TRAP::Math::tVec3<T>(inf, 1.0f, 1.0f));
        REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(t[3])));
        REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(t2[3])));
    }
    {
        const auto t = TRAP::Math::Translate(TRAP::Math::tMat4<T>(1.0f), TRAP::Math::tVec3<T>(ninf, 1.0f, 1.0f));
        const auto t2 = TRAP::Math::Translate(TRAP::Math::tVec3<T>(ninf, 1.0f, 1.0f));
        REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(t[3])));
        REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(t2[3])));
    }
    {
        const auto t = TRAP::Math::Translate(TRAP::Math::tMat4<T>(1.0f), TRAP::Math::tVec3<T>(nan, 1.0f, 1.0f));
        const auto t2 = TRAP::Math::Translate(TRAP::Math::tVec3<T>(nan, 1.0f, 1.0f));
        REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(t[3])));
        REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(t2[3])));
    }
}

TEST_CASE("TRAP::Math::Translate()", "[math][generic][translate]")
{
    SECTION("Scalar - double")
    {
        RunCompileTimeTranslateTests<double>();
        RunTranslateEdgeTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunCompileTimeTranslateTests<float>();
        RunTranslateEdgeTests<float>();
    }
}

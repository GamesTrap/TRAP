#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>
#include <array>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires std::floating_point<T>
    consteval void RunAngleCompileTimeTests()
    {
        {
            constexpr TRAP::Math::tQuat<T> q(TRAP::Math::tVec3<T>(1.0f, 0.0f, 0.0f), TRAP::Math::tVec3<T>(0.0f, 1.0f, 0.0f));
            constexpr T a = TRAP::Math::Degrees(TRAP::Math::Angle(q));
            static_assert(TRAP::Math::Equal(a, T(90.0f), T(0.0000000000001f)));
        }
        {
            constexpr TRAP::Math::tQuat<T> q(TRAP::Math::tVec3<T>(0.0f, 1.0f, 0.0f), TRAP::Math::tVec3<T>(1.0f, 0.0f, 0.0f));
            constexpr T a = TRAP::Math::Degrees(TRAP::Math::Angle(q));
            static_assert(TRAP::Math::Equal(a, T(90.0f), T(0.0000000000001f)));
        }
        if constexpr(std::same_as<T, float>)
        {
            constexpr TRAP::Math::tQuat<T> q = TRAP::Math::AngleAxis(TRAP::Math::TwoPI<T>() - T(1.0f), TRAP::Math::tVec3<T>(1.0f, 0.0f, 0.0f));
            constexpr float a = TRAP::Math::Angle(q);
            static_assert(TRAP::Math::Equal(a, 5.28f, 0.01f));
        }

        {
            constexpr TRAP::Math::tQuat<T> q = TRAP::Math::AngleAxis(TRAP::Math::PI<T>() * T(0.25f), TRAP::Math::tVec3<T>(0.0f, 0.0f, 1.0f));
            constexpr TRAP::Math::tQuat<T> n = TRAP::Math::Normalize(q);
            constexpr T l = TRAP::Math::Length(n);
            static_assert(TRAP::Math::Equal(l, T(1.0f), T(0.01f)));
            constexpr T a = TRAP::Math::Angle(n);
            static_assert(TRAP::Math::Equal(a, TRAP::Math::PI<T>() * T(0.25f), T(0.01f)));
        }
        {
            constexpr TRAP::Math::tQuat<T> q = TRAP::Math::AngleAxis(TRAP::Math::PI<T>() * T(0.25f), TRAP::Math::Normalize(TRAP::Math::tVec3<T>(0.0f, 1.0f, 1.0f)));
            constexpr TRAP::Math::tQuat<T> n = TRAP::Math::Normalize(q);
            constexpr T l = TRAP::Math::Length(n);
            static_assert(TRAP::Math::Equal(l, T(1.0f), T(0.01f)));
            constexpr T a = TRAP::Math::Angle(n);
            static_assert(TRAP::Math::Equal(a, TRAP::Math::PI<T>() * T(0.25f), T(0.01f)));
        }
        {
            constexpr TRAP::Math::tQuat<T> q = TRAP::Math::AngleAxis(TRAP::Math::PI<T>() * T(0.25f), TRAP::Math::Normalize(TRAP::Math::tVec3<T>(1.0f, 2.0f, 3.0f)));
            constexpr TRAP::Math::tQuat<T> n = TRAP::Math::Normalize(q);
            constexpr T l = TRAP::Math::Length(n);
            static_assert(TRAP::Math::Equal(l, T(1.0f), T(0.01f)));
            constexpr T a = TRAP::Math::Angle(n);
            static_assert(TRAP::Math::Equal(a, TRAP::Math::PI<T>() * T(0.25f), T(0.01f)));
        }
    }

    template<typename T>
    requires std::floating_point<T>
    void RunAngleRunTimeTests()
    {
        {
            const TRAP::Math::tQuat<T> q(TRAP::Math::tVec3<T>(1.0f, 0.0f, 0.0f), TRAP::Math::tVec3<T>(0.0f, 1.0f, 0.0f));
            const T a = TRAP::Math::Degrees(TRAP::Math::Angle(q));
            REQUIRE(TRAP::Math::Equal(a, T(90.0f), T(0.0000000000001f)));
        }
        {
            const TRAP::Math::tQuat<T> q(TRAP::Math::tVec3<T>(0.0f, 1.0f, 0.0f), TRAP::Math::tVec3<T>(1.0f, 0.0f, 0.0f));
            const T a = TRAP::Math::Degrees(TRAP::Math::Angle(q));
            REQUIRE(TRAP::Math::Equal(a, T(90.0f), T(0.0000000000001f)));
        }
        if constexpr(std::same_as<T, float>)
        {
            const TRAP::Math::tQuat<T> q = TRAP::Math::AngleAxis(TRAP::Math::TwoPI<T>() - T(1.0f), TRAP::Math::tVec3<T>(1.0f, 0.0f, 0.0f));
            const T a = TRAP::Math::Angle(q);
            REQUIRE(TRAP::Math::Equal(a, T(5.28319f), T(0.00001f)));
        }

        {
            const TRAP::Math::tQuat<T> q = TRAP::Math::AngleAxis(TRAP::Math::PI<T>() * T(0.25f), TRAP::Math::tVec3<T>(0.0f, 0.0f, 1.0f));
            const TRAP::Math::tQuat<T> n = TRAP::Math::Normalize(q);
            const T l = TRAP::Math::Length(n);
            REQUIRE(TRAP::Math::Equal(l, T(1.0f), T(0.01f)));
            const T a = TRAP::Math::Angle(n);
            REQUIRE(TRAP::Math::Equal(a, TRAP::Math::PI<T>() * T(0.25f), T(0.01f)));
        }
        {
            const TRAP::Math::tQuat<T> q = TRAP::Math::AngleAxis(TRAP::Math::PI<T>() * T(0.25f), TRAP::Math::Normalize(TRAP::Math::tVec3<T>(0.0f, 1.0f, 1.0f)));
            const TRAP::Math::tQuat<T> n = TRAP::Math::Normalize(q);
            const T l = TRAP::Math::Length(n);
            REQUIRE(TRAP::Math::Equal(l, T(1.0f), T(0.01f)));
            const T a = TRAP::Math::Angle(n);
            REQUIRE(TRAP::Math::Equal(a, TRAP::Math::PI<T>() * T(0.25f), T(0.01f)));
        }
        {
            const TRAP::Math::tQuat<T> q = TRAP::Math::AngleAxis(TRAP::Math::PI<T>() * T(0.25f), TRAP::Math::Normalize(TRAP::Math::tVec3<T>(1.0f, 2.0f, 3.0f)));
            const TRAP::Math::tQuat<T> n = TRAP::Math::Normalize(q);
            const T l = TRAP::Math::Length(n);
            REQUIRE(TRAP::Math::Equal(l, T(1.0f), T(0.01f)));
            const T a = TRAP::Math::Angle(n);
            REQUIRE(TRAP::Math::Equal(a, TRAP::Math::PI<T>() * T(0.25f), T(0.01f)));
        }
    }

    template<typename T>
    requires std::floating_point<T>
    void RunAngleEdgeTests()
    {
        static constexpr T min = std::numeric_limits<T>::lowest();
        static constexpr T max = std::numeric_limits<T>::max();
        static constexpr T inf = std::numeric_limits<T>::infinity();
        static constexpr T ninf = -std::numeric_limits<T>::infinity();
        static constexpr T nan = std::numeric_limits<T>::quiet_NaN();

        {
            const TRAP::Math::tQuat<T> q(min, min, min, min);
            const T a = TRAP::Math::Degrees(TRAP::Math::Angle(q));
            REQUIRE(TRAP::Math::IsNaN(a));
        }
        {
            const TRAP::Math::tQuat<T> q(max, max, max, max);
            const T a = TRAP::Math::Degrees(TRAP::Math::Angle(q));
            REQUIRE(TRAP::Math::IsNaN(a));
        }
        {
            static constexpr TRAP::Math::tQuat<T> q(inf, inf, inf, inf);
            const T a = TRAP::Math::Degrees(TRAP::Math::Angle(q));
            REQUIRE(TRAP::Math::IsNaN(a));
        }
        {
            static constexpr TRAP::Math::tQuat<T> q(ninf, ninf, ninf, ninf);
            const T a = TRAP::Math::Degrees(TRAP::Math::Angle(q));
            REQUIRE(TRAP::Math::IsNaN(a));
        }
        {
            static constexpr TRAP::Math::tQuat<T> q(nan, nan, nan, nan);
            const T a = TRAP::Math::Degrees(TRAP::Math::Angle(q));
            REQUIRE(TRAP::Math::IsNaN(a));
        }
    }
}

TEST_CASE("TRAP::Math::Angle()", "[math][generic][angle]")
{
    SECTION("Quat - double")
    {
        RunAngleRunTimeTests<double>();
        RunAngleCompileTimeTests<double>();
        RunAngleEdgeTests<double>();
    }
    SECTION("Quat - float")
    {
        RunAngleRunTimeTests<float>();
        RunAngleCompileTimeTests<float>();
        RunAngleEdgeTests<float>();
    }
}
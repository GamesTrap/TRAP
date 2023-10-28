#include <limits>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires std::floating_point<T>
    consteval void RunRollCompileTimeTests()
    {
        constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        {
            constexpr T expected = 0.0f;
            constexpr TRAP::Math::tQuat<T> q(TRAP::Math::Radians(TRAP::Math::tVec3<T>(0.0f, 0.0f, expected)));
            constexpr T roll = TRAP::Math::Degrees(TRAP::Math::Roll(q));
            static_assert(TRAP::Math::Equal(roll, expected, Epsilon));
        }
        {
            constexpr T expected = 90.0f;
            constexpr TRAP::Math::tQuat<T> q(TRAP::Math::Radians(TRAP::Math::tVec3<T>(0.0f, 0.0f, expected)));
            constexpr T roll = TRAP::Math::Degrees(TRAP::Math::Roll(q));
            static_assert(TRAP::Math::Equal(roll, expected, T(0.0000000000001f)));
        }
        {
            constexpr T expected = 45.0f;
            constexpr TRAP::Math::tQuat<T> q(TRAP::Math::Radians(TRAP::Math::tVec3<T>(0.0f, 0.0f, expected)));
            constexpr T roll = TRAP::Math::Degrees(TRAP::Math::Roll(q));
            static_assert(TRAP::Math::Equal(roll, expected, T(0.00001f)));
        }
        {
            constexpr T expected = 180.0f;
            constexpr TRAP::Math::tQuat<T> q(TRAP::Math::Radians(TRAP::Math::tVec3<T>(0.0f, 0.0f, expected)));
            constexpr T roll = TRAP::Math::Abs(TRAP::Math::Degrees(TRAP::Math::Roll(q)));
            static_assert(TRAP::Math::Equal(roll, expected, Epsilon));
        }
        {
            constexpr T expected = -45.0f;
            constexpr TRAP::Math::tQuat<T> q(TRAP::Math::Radians(TRAP::Math::tVec3<T>(0.0f, 0.0f, expected)));
            constexpr T roll = TRAP::Math::Degrees(TRAP::Math::Roll(q));
            static_assert(TRAP::Math::Equal(roll, expected, T(0.00001f)));
        }
    }

    template<typename T>
    requires std::floating_point<T>
    void RunRollRunTimeTests()
    {
        static constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        {
            const T expected = 0.0f;
            const TRAP::Math::tQuat<T> q(TRAP::Math::Radians(TRAP::Math::tVec3<T>(0.0f, 0.0f, expected)));
            const T roll = TRAP::Math::Degrees(TRAP::Math::Roll(q));
            REQUIRE(TRAP::Math::Equal(roll, expected, Epsilon));
        }
        {
            const T expected = 90.0f;
            const TRAP::Math::tQuat<T> q(TRAP::Math::Radians(TRAP::Math::tVec3<T>(0.0f, 0.0f, expected)));
            const T roll = TRAP::Math::Degrees(TRAP::Math::Roll(q));
            REQUIRE(TRAP::Math::Equal(roll, expected, T(0.0000000000001f)));
        }
        {
            const T expected = 45.0f;
            const TRAP::Math::tQuat<T> q(TRAP::Math::Radians(TRAP::Math::tVec3<T>(0.0f, 0.0f, expected)));
            const T roll = TRAP::Math::Degrees(TRAP::Math::Roll(q));
            REQUIRE(TRAP::Math::Equal(roll, expected, T(0.00001f)));
        }
        {
            const T expected = 180.0f;
            const TRAP::Math::tQuat<T> q(TRAP::Math::Radians(TRAP::Math::tVec3<T>(0.0f, 0.0f, expected)));
            const T roll = TRAP::Math::Abs(TRAP::Math::Degrees(TRAP::Math::Roll(q)));
            REQUIRE(TRAP::Math::Equal(roll, expected, Epsilon));
        }
        {
            const T expected = -45.0f;
            const TRAP::Math::tQuat<T> q(TRAP::Math::Radians(TRAP::Math::tVec3<T>(0.0f, 0.0f, expected)));
            const T roll = TRAP::Math::Degrees(TRAP::Math::Roll(q));
            REQUIRE(TRAP::Math::Equal(roll, expected, T(0.00001f)));
        }
    }

    template<typename T>
    requires std::floating_point<T>
    void RunRollEdgeTests()
    {
        static constexpr T inf = std::numeric_limits<T>::infinity();
        static constexpr T ninf = -std::numeric_limits<T>::infinity();
        static constexpr T nan = std::numeric_limits<T>::quiet_NaN();

        {
            const T expected = inf;
            const TRAP::Math::tQuat<T> q(TRAP::Math::Radians(TRAP::Math::tVec3<T>(0.0f, 0.0f, expected)));
            const T roll = TRAP::Math::Degrees(TRAP::Math::Roll(q));
            REQUIRE(TRAP::Math::IsNaN(roll));
        }
        {
            const T expected = ninf;
            const TRAP::Math::tQuat<T> q(TRAP::Math::Radians(TRAP::Math::tVec3<T>(0.0f, 0.0f, expected)));
            const T roll = TRAP::Math::Degrees(TRAP::Math::Roll(q));
            REQUIRE(TRAP::Math::IsNaN(roll));
        }
        {
            const T expected = nan;
            const TRAP::Math::tQuat<T> q(TRAP::Math::Radians(TRAP::Math::tVec3<T>(0.0f, 0.0f, expected)));
            const T roll = TRAP::Math::Degrees(TRAP::Math::Roll(q));
            REQUIRE(TRAP::Math::IsNaN(roll));
        }
    }
}

TEST_CASE("TRAP::Math::Roll()", "[math][generic][roll]")
{
    SECTION("Quat - f64")
    {
        RunRollRunTimeTests<f64>();
        RunRollCompileTimeTests<f64>();
        RunRollEdgeTests<f64>();
    }
    SECTION("Quat - f32")
    {
        RunRollRunTimeTests<f32>();
        RunRollCompileTimeTests<f32>();
        RunRollEdgeTests<f32>();
    }
}
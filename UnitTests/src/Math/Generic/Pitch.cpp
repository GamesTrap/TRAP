#include <limits>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires std::floating_point<T>
    consteval void RunPitchCompileTimeTests()
    {
        constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        {
            constexpr T expected = 0.0f;
            constexpr TRAP::Math::tQuat<T> q(TRAP::Math::Radians(TRAP::Math::tVec3<T>(expected, 0.0f, 0.0f)));
            constexpr T pitch = TRAP::Math::Degrees(TRAP::Math::Pitch(q));
            static_assert(TRAP::Math::Equal(pitch, expected, Epsilon));
        }
        {
            constexpr T expected = 90.0f;
            constexpr TRAP::Math::tQuat<T> q(TRAP::Math::Radians(TRAP::Math::tVec3<T>(expected, 0.0f, 0.0f)));
            constexpr T pitch = TRAP::Math::Degrees(TRAP::Math::Pitch(q));
            static_assert(TRAP::Math::Equal(pitch, expected, T(0.0000000000001f)));
        }
        {
            constexpr T expected = 45.0f;
            constexpr TRAP::Math::tQuat<T> q(TRAP::Math::Radians(TRAP::Math::tVec3<T>(expected, 0.0f, 0.0f)));
            constexpr T pitch = TRAP::Math::Degrees(TRAP::Math::Pitch(q));
            static_assert(TRAP::Math::Equal(pitch, expected, T(0.00001f)));
        }
        {
            constexpr T expected = 180.0f;
            constexpr TRAP::Math::tQuat<T> q(TRAP::Math::Radians(TRAP::Math::tVec3<T>(expected, 0.0f, 0.0f)));
            constexpr T pitch = TRAP::Math::Abs(TRAP::Math::Degrees(TRAP::Math::Pitch(q)));
            static_assert(TRAP::Math::Equal(pitch, expected, Epsilon));
        }
        {
            constexpr T expected = -45.0f;
            constexpr TRAP::Math::tQuat<T> q(TRAP::Math::Radians(TRAP::Math::tVec3<T>(expected, 0.0f, 0.0f)));
            constexpr T pitch = TRAP::Math::Degrees(TRAP::Math::Pitch(q));
            static_assert(TRAP::Math::Equal(pitch, expected, T(0.00001f)));
        }
    }

    template<typename T>
    requires std::floating_point<T>
    void RunPitchRunTimeTests()
    {
        static constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        {
            const T expected = 0.0f;
            const TRAP::Math::tQuat<T> q(TRAP::Math::Radians(TRAP::Math::tVec3<T>(expected, 0.0f, 0.0f)));
            const T pitch = TRAP::Math::Degrees(TRAP::Math::Pitch(q));
            REQUIRE(TRAP::Math::Equal(pitch, expected, Epsilon));
        }
        {
            const T expected = 90.0f;
            const TRAP::Math::tQuat<T> q(TRAP::Math::Radians(TRAP::Math::tVec3<T>(expected, 0.0f, 0.0f)));
            const T pitch = TRAP::Math::Degrees(TRAP::Math::Pitch(q));
            REQUIRE(TRAP::Math::Equal(pitch, expected, T(0.0000000000001f)));
        }
        {
            const T expected = 45.0f;
            const TRAP::Math::tQuat<T> q(TRAP::Math::Radians(TRAP::Math::tVec3<T>(expected, 0.0f, 0.0f)));
            const T pitch = TRAP::Math::Degrees(TRAP::Math::Pitch(q));
            REQUIRE(TRAP::Math::Equal(pitch, expected, T(0.00001f)));
        }
        {
            const T expected = 180.0f;
            const TRAP::Math::tQuat<T> q(TRAP::Math::Radians(TRAP::Math::tVec3<T>(expected, 0.0f, 0.0f)));
            const T pitch = TRAP::Math::Abs(TRAP::Math::Degrees(TRAP::Math::Pitch(q)));
            REQUIRE(TRAP::Math::Equal(pitch, expected, Epsilon));
        }
        {
            const T expected = -45.0f;
            const TRAP::Math::tQuat<T> q(TRAP::Math::Radians(TRAP::Math::tVec3<T>(expected, 0.0f, 0.0f)));
            const T pitch = TRAP::Math::Degrees(TRAP::Math::Pitch(q));
            REQUIRE(TRAP::Math::Equal(pitch, expected, T(0.00001f)));
        }
    }

    template<typename T>
    requires std::floating_point<T>
    void RunPitchEdgeTests()
    {
        static constexpr T inf = std::numeric_limits<T>::infinity();
        static constexpr T ninf = -std::numeric_limits<T>::infinity();
        static constexpr T nan = std::numeric_limits<T>::quiet_NaN();

        {
            const T expected = inf;
            const TRAP::Math::tQuat<T> q(TRAP::Math::Radians(TRAP::Math::tVec3<T>(expected, 0.0f, 0.0f)));
            const T pitch = TRAP::Math::Degrees(TRAP::Math::Pitch(q));
            REQUIRE(TRAP::Math::IsNaN(pitch));
        }
        {
            const T expected = ninf;
            const TRAP::Math::tQuat<T> q(TRAP::Math::Radians(TRAP::Math::tVec3<T>(expected, 0.0f, 0.0f)));
            const T pitch = TRAP::Math::Degrees(TRAP::Math::Pitch(q));
            REQUIRE(TRAP::Math::IsNaN(pitch));
        }
        {
            const T expected = nan;
            const TRAP::Math::tQuat<T> q(TRAP::Math::Radians(TRAP::Math::tVec3<T>(expected, 0.0f, 0.0f)));
            const T pitch = TRAP::Math::Degrees(TRAP::Math::Pitch(q));
            REQUIRE(TRAP::Math::IsNaN(pitch));
        }
    }
}

TEST_CASE("TRAP::Math::Pitch()", "[math][generic][pitch]")
{
    SECTION("Quat - double")
    {
        RunPitchRunTimeTests<double>();
        RunPitchCompileTimeTests<double>();
        RunPitchEdgeTests<double>();
    }
    SECTION("Quat - float")
    {
        RunPitchRunTimeTests<float>();
        RunPitchCompileTimeTests<float>();
        RunPitchEdgeTests<float>();
    }
}
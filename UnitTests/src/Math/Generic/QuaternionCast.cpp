#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>
#include <array>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires (TRAP::Math::IsMat<T> && std::floating_point<typename T::value_type>)
void RunQuaternionCastTests()
{
    constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

    {
        constexpr T m(1.0f);
        const TRAP::Math::tQuat<typename T::value_type> res = TRAP::Math::QuaternionCast(m);
        constexpr TRAP::Math::tQuat<typename T::value_type> expected(1.0f, 0.0f, 0.0f, 0.0f);
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(res, expected, Epsilon)));
    }
    {
        constexpr T rot(TRAP::Math::tMat4<typename T::value_type>(0.866f, -0.5f, 0.0f, 0.0f, 0.5f, 0.866f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f));
        const TRAP::Math::tQuat<typename T::value_type> res = TRAP::Math::QuaternionCast(rot);
        constexpr TRAP::Math::tQuat<typename T::value_type> expected(0.965919f, 0.0f, 0.0f, -0.258821f);
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(res, expected, typename T::value_type(0.000001f))));
    }

    if constexpr(TRAP::Math::IsMat4<T>)
    {
        {
            constexpr typename T::value_type angle = TRAP::Math::Radians(45.0f);
            const T rot = TRAP::Math::Rotate(T(1.0f), angle, TRAP::Math::tVec3<typename T::value_type>(1.0f, 0.0f, 0.0f));
            const TRAP::Math::tQuat<typename T::value_type> res = TRAP::Math::QuaternionCast(rot);
            constexpr TRAP::Math::tQuat<typename T::value_type> expected(0.923880f, 0.382683f, 0.0f, 0.0f);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(res, expected, typename T::value_type(0.000001f))));
        }
        {
            constexpr typename T::value_type angle = TRAP::Math::Radians(90.0f);
            const T rot = TRAP::Math::Rotate(T(1.0f), angle, TRAP::Math::tVec3<typename T::value_type>(0.0f, 1.0f, 0.0f));
            const TRAP::Math::tQuat<typename T::value_type> res = TRAP::Math::QuaternionCast(rot);
            constexpr TRAP::Math::tQuat<typename T::value_type> expected(0.707107f, 0.0f, 0.707107f, 0.0f);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(res, expected, typename T::value_type(0.000001f))));
        }
        {
            constexpr typename T::value_type angle = TRAP::Math::Radians(180.0f);
            const T rot = TRAP::Math::Rotate(T(1.0f), angle, TRAP::Math::tVec3<typename T::value_type>(0.0f, 0.0f, 1.0f));
            const TRAP::Math::tQuat<typename T::value_type> res = TRAP::Math::QuaternionCast(rot);
            constexpr TRAP::Math::tQuat<typename T::value_type> expected(0.0f, 0.0f, 0.0f, 1.0f);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(res, expected, typename T::value_type(0.000001f))));
        }
    }
}

template<typename T>
requires (TRAP::Math::IsMat<T> && std::floating_point<typename T::value_type>)
void RunQuaternionCastEdgeTests()
{
    constexpr typename T::value_type min = std::numeric_limits<typename T::value_type>::lowest();
    constexpr typename T::value_type max = std::numeric_limits<typename T::value_type>::max();
    constexpr typename T::value_type inf = std::numeric_limits<typename T::value_type>::infinity();
    constexpr typename T::value_type ninf = -std::numeric_limits<typename T::value_type>::infinity();
    constexpr typename T::value_type nan = std::numeric_limits<typename T::value_type>::quiet_NaN();

    {
        constexpr T m(TRAP::Math::tMat4<typename T::value_type>(min, min, min, min, min, min, min, min, min, min, min, min, min, min, min, min));
        const TRAP::Math::tQuat<typename T::value_type> res = TRAP::Math::QuaternionCast(m);
        REQUIRE(TRAP::Math::Any(TRAP::Math::IsInf(res)));
    }
    {
        constexpr T m(TRAP::Math::tMat4<typename T::value_type>(max, max, max, max, max, max, max, max, max, max, max, max, max, max, max, max));
        const TRAP::Math::tQuat<typename T::value_type> res = TRAP::Math::QuaternionCast(m);
        REQUIRE(TRAP::Math::Any(TRAP::Math::IsInf(res)));
    }
    {
        constexpr T m(TRAP::Math::tMat4<typename T::value_type>(inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf));
        const TRAP::Math::tQuat<typename T::value_type> res = TRAP::Math::QuaternionCast(m);
        REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(res)));
    }
    {
        constexpr T m(TRAP::Math::tMat4<typename T::value_type>(ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf));
        const TRAP::Math::tQuat<typename T::value_type> res = TRAP::Math::QuaternionCast(m);
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(res)));
    }
    {
        constexpr T m(TRAP::Math::tMat4<typename T::value_type>(nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan));
        const TRAP::Math::tQuat<typename T::value_type> res = TRAP::Math::QuaternionCast(m);
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(res)));
    }
}

TEST_CASE("TRAP::Math::QuaternionCast()", "[math][generic][quaternioncast]")
{
    SECTION("Mat3 - double")
    {
        RunQuaternionCastTests<TRAP::Math::Mat3d>();
        RunQuaternionCastEdgeTests<TRAP::Math::Mat3d>();
    }
    SECTION("Mat3 - float")
    {
        RunQuaternionCastTests<TRAP::Math::Mat3f>();
        RunQuaternionCastEdgeTests<TRAP::Math::Mat3f>();
    }

    SECTION("Mat4 - double")
    {
        RunQuaternionCastTests<TRAP::Math::Mat4d>();
        RunQuaternionCastEdgeTests<TRAP::Math::Mat4d>();
    }
    SECTION("Mat4 - float")
    {
        RunQuaternionCastTests<TRAP::Math::Mat4f>();
        RunQuaternionCastEdgeTests<TRAP::Math::Mat4f>();
    }
}

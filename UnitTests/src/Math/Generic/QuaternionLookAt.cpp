#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>
#include <array>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::floating_point<T>
void RunQuaternionLookAtTests()
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    constexpr TRAP::Math::tVec3<T> eye(0.0f);
    constexpr TRAP::Math::tVec3<T> center(1.1f, -2.0f, 3.1416f);
    constexpr TRAP::Math::tVec3<T> up(-0.17f, 7.23f, -1.744f);

    const TRAP::Math::tQuat<T> q = TRAP::Math::QuaternionLookAt(TRAP::Math::Normalize(center - eye), up);
    const TRAP::Math::tQuat<T> m = TRAP::Math::Conjugate(TRAP::Math::QuaternionCast(TRAP::Math::LookAt(eye, center, up)));

    REQUIRE(TRAP::Math::Equal(TRAP::Math::Length(q), T(1.0f), Epsilon));
    REQUIRE(TRAP::Math::All(TRAP::Math::Equal(-q, m, Epsilon)));
}

template<typename T>
requires std::floating_point<T>
void RunQuaternionLookAtEdgeTests()
{
    constexpr T min = std::numeric_limits<T>::lowest();
    constexpr T max = std::numeric_limits<T>::max();
    constexpr T inf = std::numeric_limits<T>::infinity();
    constexpr T ninf = -std::numeric_limits<T>::infinity();
    constexpr T nan = std::numeric_limits<T>::quiet_NaN();

    {
        constexpr TRAP::Math::tVec3<T> eye(min);
        constexpr TRAP::Math::tVec3<T> target(min);
        const TRAP::Math::tQuat<T> res = TRAP::Math::QuaternionLookAt(eye, target);
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(res)));
    }
    {
        constexpr TRAP::Math::tVec3<T> eye(max);
        constexpr TRAP::Math::tVec3<T> target(max);
        const TRAP::Math::tQuat<T> res = TRAP::Math::QuaternionLookAt(eye, target);
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(res)));
    }
    {
        constexpr TRAP::Math::tVec3<T> eye(inf);
        constexpr TRAP::Math::tVec3<T> target(inf);
        const TRAP::Math::tQuat<T> res = TRAP::Math::QuaternionLookAt(eye, target);
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(res)));
    }
    {
        constexpr TRAP::Math::tVec3<T> eye(ninf);
        constexpr TRAP::Math::tVec3<T> target(ninf);
        const TRAP::Math::tQuat<T> res = TRAP::Math::QuaternionLookAt(eye, target);
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(res)));
    }
    {
        constexpr TRAP::Math::tVec3<T> eye(nan);
        constexpr TRAP::Math::tVec3<T> target(nan);
        const TRAP::Math::tQuat<T> res = TRAP::Math::QuaternionLookAt(eye, target);
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(res)));
    }
}

TEST_CASE("TRAP::Math::QuaternionLookAt()", "[math][generic][quaternionlookat]")
{
    SECTION("Quat - double")
    {
        RunQuaternionLookAtTests<double>();
        RunQuaternionLookAtEdgeTests<double>();
    }
    SECTION("Quat - float")
    {
        RunQuaternionLookAtTests<float>();
        RunQuaternionLookAtEdgeTests<float>();
    }
}

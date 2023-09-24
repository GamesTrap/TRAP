#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>
#include <array>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::floating_point<T>
consteval void RunQuaternionLookAtCompileTimeTests()
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    constexpr TRAP::Math::tVec3<T> eye(0.0f);
    constexpr TRAP::Math::tVec3<T> center(1.1f, -2.0f, 3.1416f);
    constexpr TRAP::Math::tVec3<T> up(-0.17f, 7.23f, -1.744f);

    constexpr TRAP::Math::tQuat<T> q = TRAP::Math::QuaternionLookAt(TRAP::Math::Normalize(center - eye), up);
    constexpr TRAP::Math::tQuat<T> m = TRAP::Math::Conjugate(TRAP::Math::QuaternionCast(TRAP::Math::LookAt(eye, center, up)));

    static_assert(TRAP::Math::Equal(TRAP::Math::Length(q), T(1.0f), Epsilon));
    static_assert(TRAP::Math::All(TRAP::Math::Equal(-q, m, Epsilon)));
}

template<typename T>
requires std::floating_point<T>
void RunQuaternionLookAtRunTimeTests()
{
    static constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    static constexpr TRAP::Math::tVec3<T> eye(0.0f);
    static constexpr TRAP::Math::tVec3<T> center(1.1f, -2.0f, 3.1416f);
    static constexpr TRAP::Math::tVec3<T> up(-0.17f, 7.23f, -1.744f);

    const TRAP::Math::tQuat<T> q = TRAP::Math::QuaternionLookAt(TRAP::Math::Normalize(center - eye), up);
    const TRAP::Math::tQuat<T> m = TRAP::Math::Conjugate(TRAP::Math::QuaternionCast(TRAP::Math::LookAt(eye, center, up)));

    REQUIRE(TRAP::Math::Equal(TRAP::Math::Length(q), T(1.0f), Epsilon));
    REQUIRE(TRAP::Math::All(TRAP::Math::Equal(-q, m, Epsilon)));
}

template<typename T>
requires std::floating_point<T>
void RunQuaternionLookAtEdgeTests()
{
    static constexpr T min = std::numeric_limits<T>::lowest();
    static constexpr T max = std::numeric_limits<T>::max();
    static constexpr T inf = std::numeric_limits<T>::infinity();
    static constexpr T ninf = -std::numeric_limits<T>::infinity();
    static constexpr T nan = std::numeric_limits<T>::quiet_NaN();

    {
        static constexpr TRAP::Math::tVec3<T> eye(min);
        static constexpr TRAP::Math::tVec3<T> target(min);
        const TRAP::Math::tQuat<T> res = TRAP::Math::QuaternionLookAt(eye, target);
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(res)));
    }
    {
        static constexpr TRAP::Math::tVec3<T> eye(max);
        static constexpr TRAP::Math::tVec3<T> target(max);
        const TRAP::Math::tQuat<T> res = TRAP::Math::QuaternionLookAt(eye, target);
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(res)));
    }
    {
        static constexpr TRAP::Math::tVec3<T> eye(inf);
        static constexpr TRAP::Math::tVec3<T> target(inf);
        const TRAP::Math::tQuat<T> res = TRAP::Math::QuaternionLookAt(eye, target);
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(res)));
    }
    {
        static constexpr TRAP::Math::tVec3<T> eye(ninf);
        static constexpr TRAP::Math::tVec3<T> target(ninf);
        const TRAP::Math::tQuat<T> res = TRAP::Math::QuaternionLookAt(eye, target);
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(res)));
    }
    {
        static constexpr TRAP::Math::tVec3<T> eye(nan);
        static constexpr TRAP::Math::tVec3<T> target(nan);
        const TRAP::Math::tQuat<T> res = TRAP::Math::QuaternionLookAt(eye, target);
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(res)));
    }
}

TEST_CASE("TRAP::Math::QuaternionLookAt()", "[math][generic][quaternionlookat]")
{
    SECTION("Quat - double")
    {
        RunQuaternionLookAtRunTimeTests<double>();
        RunQuaternionLookAtCompileTimeTests<double>();
        RunQuaternionLookAtEdgeTests<double>();
    }
    SECTION("Quat - float")
    {
        RunQuaternionLookAtRunTimeTests<float>();
        RunQuaternionLookAtCompileTimeTests<float>();
        RunQuaternionLookAtEdgeTests<float>();
    }
}

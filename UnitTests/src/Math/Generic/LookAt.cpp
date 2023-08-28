#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>
#include <array>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::floating_point<T>
void RunLookAtTests()
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    {
        constexpr TRAP::Math::tVec3<T> eye(0.0f);
        constexpr TRAP::Math::tVec3<T> center(0.0f, 0.0f, -1.0f);
        constexpr TRAP::Math::tVec3<T> up(0.0f, 1.0f, 0.0f);

        const TRAP::Math::tMat4<T> res = TRAP::Math::LookAt(eye, center, up);
        constexpr TRAP::Math::tMat4<T> expected(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(res, expected, Epsilon)));
    }
    {
        constexpr TRAP::Math::tVec3<T> eye(2.0f);
        constexpr TRAP::Math::tVec3<T> center(0.0f);
        constexpr TRAP::Math::tVec3<T> up(0.0f, 1.0f, 0.0f);

        const TRAP::Math::tMat4<T> res = TRAP::Math::LookAt(eye, center, up);
        constexpr TRAP::Math::tMat4<T> expected(0.707107f, -0.408248f, 0.577350f, 0.0f, 0.0f, 0.816497f, 0.577350f, 0.0f, -0.707107f, -0.408248f, 0.577350f, 0.0f, 0.0f, 0.0f, -3.464102f, 1.0f);
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(res, expected, T(0.000001f))));
    }
    {
        constexpr TRAP::Math::tVec3<T> eye(-1.0f, 1.0f, 1.0f);
        constexpr TRAP::Math::tVec3<T> center(1.0f, -1.0f, -1.0f);
        constexpr TRAP::Math::tVec3<T> up(0.0f, 1.0f, 0.0f);

        const TRAP::Math::tMat4<T> res = TRAP::Math::LookAt(eye, center, up);
        constexpr TRAP::Math::tMat4<T> expected(0.707107f, 0.408248f, -0.577350f, 0.0f, 0.0f, 0.816497f, 0.577350f, 0.0f, 0.707107f, -0.408248f, 0.577350, 0.0f, 0.0f, 0.0f, -1.732051f, 1.0f);
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(res, expected, T(0.000001f))));
    }
}

template<typename T>
requires std::floating_point<T>
void RunLookAtEdgeTests()
{
    constexpr T min = std::numeric_limits<T>::lowest();
    constexpr T max = std::numeric_limits<T>::max();
    constexpr T inf = std::numeric_limits<T>::infinity();
    constexpr T ninf = -std::numeric_limits<T>::infinity();
    constexpr T nan = std::numeric_limits<T>::quiet_NaN();

    {
        constexpr TRAP::Math::tVec3<T> eye(min, 0.0f, 0.0f);
        constexpr TRAP::Math::tVec3<T> center(0.0f, 0.0f, -1.0f);
        constexpr TRAP::Math::tVec3<T> up(0.0f, 1.0f, 0.0f);

        const TRAP::Math::tMat4<T> res = TRAP::Math::LookAt(eye, center, up);
        REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<0>(res))));
    }
    {
        constexpr TRAP::Math::tVec3<T> eye(max, 0.0f, 0.0f);
        constexpr TRAP::Math::tVec3<T> center(0.0f, 0.0f, -1.0f);
        constexpr TRAP::Math::tVec3<T> up(0.0f, 1.0f, 0.0f);

        const TRAP::Math::tMat4<T> res = TRAP::Math::LookAt(eye, center, up);
        REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<0>(res))));
    }
    {
        constexpr TRAP::Math::tVec3<T> eye(inf, 0.0f, 0.0f);
        constexpr TRAP::Math::tVec3<T> center(0.0f, 0.0f, -1.0f);
        constexpr TRAP::Math::tVec3<T> up(0.0f, 1.0f, 0.0f);

        const TRAP::Math::tMat4<T> res = TRAP::Math::LookAt(eye, center, up);
        REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<0>(res))));
    }
    {
        constexpr TRAP::Math::tVec3<T> eye(ninf, 0.0f, 0.0f);
        constexpr TRAP::Math::tVec3<T> center(0.0f, 0.0f, -1.0f);
        constexpr TRAP::Math::tVec3<T> up(0.0f, 1.0f, 0.0f);

        const TRAP::Math::tMat4<T> res = TRAP::Math::LookAt(eye, center, up);
        REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<0>(res))));
    }
    {
        constexpr TRAP::Math::tVec3<T> eye(nan, 0.0f, 0.0f);
        constexpr TRAP::Math::tVec3<T> center(0.0f, 0.0f, -1.0f);
        constexpr TRAP::Math::tVec3<T> up(0.0f, 1.0f, 0.0f);

        const TRAP::Math::tMat4<T> res = TRAP::Math::LookAt(eye, center, up);
        REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<0>(res))));
    }
}

TEST_CASE("TRAP::Math::LookAt()", "[math][generic][lookat]")
{
    SECTION("Scalar - double")
    {
        RunLookAtTests<double>();
        RunLookAtEdgeTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunLookAtTests<float>();
        RunLookAtEdgeTests<float>();
    }
}

#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>
#include <array>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires (TRAP::Math::IsMat<T> && std::floating_point<typename T::valueType> && T::Length() > 2)
consteval void RunCompileTimeMatrixCompMultTests()
{
    constexpr typename T::valueType Epsilon = std::numeric_limits<typename T::valueType>::epsilon();

    {
        constexpr T m(TRAP::Math::tMat4<typename T::valueType>(0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f));
        constexpr T n = TRAP::Math::MatrixCompMult(m, m);
        constexpr T expected(TRAP::Math::tMat4<typename T::valueType>(0.0f, 1.0f, 4.0f, 9.0f, 16.0f, 25.0f, 36.0f, 49.0f, 64.0f, 81.0f, 100.0f, 121.0f, 144.0f, 169.0f, 196.0f, 225.0f));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(n, expected, Epsilon)));
    }
    {
        constexpr T m(TRAP::Math::tMat4<typename T::valueType>(0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f));
        constexpr T m1(TRAP::Math::tMat4<typename T::valueType>(2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f));
        constexpr T n = TRAP::Math::MatrixCompMult(m, m1);
        constexpr T expected(TRAP::Math::tMat4<typename T::valueType>(0.0f, 2.0f, 4.0f, 6.0f, 8.0f, 10.0f, 12.0f, 14.0f, 16.0f, 18.0f, 20.0f, 22.0f, 24.0f, 26.0f, 28.0f, 30.0f));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(n, expected, Epsilon)));
    }
}

template<typename T>
requires (TRAP::Math::IsMat<T> && std::floating_point<typename T::valueType> && T::Length() > 2)
void RunMatrixCompMultEdgeTests()
{
    constexpr typename T::valueType Epsilon = std::numeric_limits<typename T::valueType>::epsilon();

    constexpr typename T::valueType min = std::numeric_limits<typename T::valueType>::lowest();
    constexpr typename T::valueType max = std::numeric_limits<typename T::valueType>::max();
    constexpr typename T::valueType inf = std::numeric_limits<typename T::valueType>::infinity();
    constexpr typename T::valueType ninf = -std::numeric_limits<typename T::valueType>::infinity();
    constexpr typename T::valueType nan = std::numeric_limits<typename T::valueType>::quiet_NaN();

    {
        constexpr T m(TRAP::Math::tMat4<typename T::valueType>{min});
        constexpr T m1(TRAP::Math::tMat4<typename T::valueType>(1.0f));
        constexpr T n = TRAP::Math::MatrixCompMult(m, m1);
        static_assert(TRAP::Math::All(TRAP::Math::Equal(n, m, Epsilon)));
    }
    {
        constexpr T m(TRAP::Math::tMat4<typename T::valueType>{max});
        constexpr T m1(TRAP::Math::tMat4<typename T::valueType>(1.0f));
        constexpr T n = TRAP::Math::MatrixCompMult(m, m1);
        static_assert(TRAP::Math::All(TRAP::Math::Equal(n, m, Epsilon)));
    }
    {
        constexpr T m(TRAP::Math::tMat4<typename T::valueType>{inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf});
        constexpr T m1(TRAP::Math::tMat4<typename T::valueType>(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f));
        constexpr T n = TRAP::Math::MatrixCompMult(m, m1);
        static_assert(TRAP::Math::All(TRAP::Math::Equal(n, m)));
    }
    {
        constexpr T m(TRAP::Math::tMat4<typename T::valueType>{ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf});
        constexpr T m1(TRAP::Math::tMat4<typename T::valueType>(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f));
        constexpr T n = TRAP::Math::MatrixCompMult(m, m1);
        static_assert(TRAP::Math::All(TRAP::Math::Equal(n, m)));
    }
    {
        constexpr T m(TRAP::Math::tMat4<typename T::valueType>{nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan});
        constexpr T m1(TRAP::Math::tMat4<typename T::valueType>(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f));
        const T n = TRAP::Math::MatrixCompMult(m, m1);
        REQUIRE(TRAP::Math::All(TRAP::Math::NotEqual(n, m)));
    }
}

TEST_CASE("TRAP::Math::MatrixCompMult()", "[math][generic][matrixcompmult]")
{
    SECTION("Mat3 - double")
    {
        RunCompileTimeMatrixCompMultTests<TRAP::Math::Mat3d>();
        RunMatrixCompMultEdgeTests<TRAP::Math::Mat3d>();
    }
    SECTION("Mat3 - float")
    {
        RunCompileTimeMatrixCompMultTests<TRAP::Math::Mat3f>();
        RunMatrixCompMultEdgeTests<TRAP::Math::Mat3f>();
    }

    SECTION("Mat4 - double")
    {
        RunCompileTimeMatrixCompMultTests<TRAP::Math::Mat4d>();
        RunMatrixCompMultEdgeTests<TRAP::Math::Mat4d>();
    }
    SECTION("Mat4 - float")
    {
        RunCompileTimeMatrixCompMultTests<TRAP::Math::Mat4f>();
        RunMatrixCompMultEdgeTests<TRAP::Math::Mat4f>();
    }
}
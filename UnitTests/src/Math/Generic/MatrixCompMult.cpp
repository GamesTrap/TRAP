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
    requires (TRAP::Math::IsMat<T> && std::floating_point<typename T::value_type> && T::Length() > 2)
    consteval void RunCompileTimeMatrixCompMultTests()
    {
        constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

        {
            constexpr T m(TRAP::Math::tMat4<typename T::value_type>(0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f));
            constexpr T n = TRAP::Math::MatrixCompMult(m, m);
            constexpr T expected(TRAP::Math::tMat4<typename T::value_type>(0.0f, 1.0f, 4.0f, 9.0f, 16.0f, 25.0f, 36.0f, 49.0f, 64.0f, 81.0f, 100.0f, 121.0f, 144.0f, 169.0f, 196.0f, 225.0f));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(n, expected, Epsilon)));
        }
        {
            constexpr T m(TRAP::Math::tMat4<typename T::value_type>(0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f));
            constexpr T m1(TRAP::Math::tMat4<typename T::value_type>(2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f));
            constexpr T n = TRAP::Math::MatrixCompMult(m, m1);
            constexpr T expected(TRAP::Math::tMat4<typename T::value_type>(0.0f, 2.0f, 4.0f, 6.0f, 8.0f, 10.0f, 12.0f, 14.0f, 16.0f, 18.0f, 20.0f, 22.0f, 24.0f, 26.0f, 28.0f, 30.0f));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(n, expected, Epsilon)));
        }
    }

    template<typename T>
    requires (TRAP::Math::IsMat<T> && std::floating_point<typename T::value_type> && T::Length() > 2)
    void RunMatrixCompMultEdgeTests()
    {
        static constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

        static constexpr typename T::value_type min = std::numeric_limits<typename T::value_type>::lowest();
        static constexpr typename T::value_type max = std::numeric_limits<typename T::value_type>::max();
        static constexpr typename T::value_type inf = std::numeric_limits<typename T::value_type>::infinity();
        static constexpr typename T::value_type ninf = -std::numeric_limits<typename T::value_type>::infinity();
        static constexpr typename T::value_type nan = std::numeric_limits<typename T::value_type>::quiet_NaN();

        {
            static constexpr T m(TRAP::Math::tMat4<typename T::value_type>{min});
            static constexpr T m1(TRAP::Math::tMat4<typename T::value_type>(1.0f));
            static constexpr T n = TRAP::Math::MatrixCompMult(m, m1);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(n, m, Epsilon)));
        }
        {
            static constexpr T m(TRAP::Math::tMat4<typename T::value_type>{max});
            static constexpr T m1(TRAP::Math::tMat4<typename T::value_type>(1.0f));
            static constexpr T n = TRAP::Math::MatrixCompMult(m, m1);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(n, m, Epsilon)));
        }
        {
            static constexpr T m(TRAP::Math::tMat4<typename T::value_type>{inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf});
            static constexpr T m1(TRAP::Math::tMat4<typename T::value_type>(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f));
            static constexpr T n = TRAP::Math::MatrixCompMult(m, m1);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(n, m)));
        }
        {
            static constexpr T m(TRAP::Math::tMat4<typename T::value_type>{ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf});
            static constexpr T m1(TRAP::Math::tMat4<typename T::value_type>(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f));
            static constexpr T n = TRAP::Math::MatrixCompMult(m, m1);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(n, m)));
        }
        {
            static constexpr T m(TRAP::Math::tMat4<typename T::value_type>{nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan});
            static constexpr T m1(TRAP::Math::tMat4<typename T::value_type>(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f));
            const T n = TRAP::Math::MatrixCompMult(m, m1);
            REQUIRE(TRAP::Math::All(TRAP::Math::NotEqual(n, m)));
        }
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
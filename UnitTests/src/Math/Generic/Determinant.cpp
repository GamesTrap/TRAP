#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>
#include <array>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires ((TRAP::Math::IsMat3<T> || TRAP::Math::IsMat4<T>) && std::floating_point<typename T::valueType>)
consteval void RunCompileTimeDeterminantTests()
{
    constexpr typename T::valueType Epsilon = std::numeric_limits<typename T::valueType>::epsilon();

    static_assert(TRAP::Math::Equal(TRAP::Math::Determinant(T(1.0f)), typename T::valueType(1.0f), Epsilon));
    static_assert(TRAP::Math::Equal(TRAP::Math::Determinant(T(0.0f)), typename T::valueType(0.0f), Epsilon));

    constexpr T input1(TRAP::Math::tMat4<typename T::valueType>(2.0f, 0.0f, 0.0f, 0.0f, 0.0f, 3.0f, 0.0f, 0.0f, 0.0f, 0.0f, 4.0f, 0.0f, 0.0f, 0.0f, 0.0f, 5.0f));
    constexpr T input2(TRAP::Math::tMat4<typename T::valueType>(2.0f, 4.0f, 1.0f, 3.0f, 1.0f, 2.0f, 4.0f, 6.0f, 3.0f, 1.0f, 2.0f, 2.0f, 5.0f, 7.0f, 3.0f, 1.0f));
    constexpr T input3(TRAP::Math::tMat4<typename T::valueType>(1.0f, 2.0f, 3.0f, 4.0f, 2.0f, 4.0f, 6.0f, 8.0f, 3.0f, 6.0f, 9.0f, 12.0f, 4.0f, 8.0f, 12.0f, 16.0f));
    constexpr T input4(TRAP::Math::tMat4<typename T::valueType>(-1.0f, 2.0f, -3.0f, 4.0f, -2.0f, 5.0f, -1.0f, 7.0f, -3.0f, 1.0f, -4.0f, 6.0f, -4.0f, 3.0f, -2.0f, 8.0f));

    if constexpr(TRAP::Math::IsMat4<T>)
    {
        static_assert(TRAP::Math::Equal(TRAP::Math::Determinant(input1), typename T::valueType(120.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Determinant(input2), typename T::valueType(-184.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Determinant(input3), typename T::valueType(0.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Determinant(input4), typename T::valueType(5.0f), Epsilon));
    }
    else if constexpr(TRAP::Math::IsMat3<T>)
    {
        static_assert(TRAP::Math::Equal(TRAP::Math::Determinant(input1), typename T::valueType(24.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Determinant(input2), typename T::valueType(35.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Determinant(input3), typename T::valueType(0.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Determinant(input4), typename T::valueType(-30.0f), Epsilon));
    }
}

template<typename T>
requires ((TRAP::Math::IsMat3<T> || TRAP::Math::IsMat4<T>) && std::floating_point<typename T::valueType>)
void RunDeterminantEdgeTests()
{
    constexpr typename T::valueType min = std::numeric_limits<typename T::valueType>::lowest();
    constexpr typename T::valueType max = std::numeric_limits<typename T::valueType>::max();
    constexpr typename T::valueType inf = std::numeric_limits<typename T::valueType>::infinity();
    constexpr typename T::valueType ninf = -std::numeric_limits<typename T::valueType>::infinity();
    constexpr typename T::valueType nan = std::numeric_limits<typename T::valueType>::quiet_NaN();

    constexpr T input1(TRAP::Math::tMat4<typename T::valueType>{min});
    constexpr T input2(TRAP::Math::tMat4<typename T::valueType>{max});
    constexpr T input3(TRAP::Math::tMat4<typename T::valueType>{inf});
    constexpr T input4(TRAP::Math::tMat4<typename T::valueType>{ninf});
    constexpr T input5(TRAP::Math::tMat4<typename T::valueType>{nan});

    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Determinant(input3)));
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Determinant(input4)));
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Determinant(input5)));

    if constexpr(TRAP::Math::IsMat4<T>)
    {
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Determinant(input1)));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Determinant(input2)));
    }
    else if constexpr(TRAP::Math::IsMat3<T>)
    {
        REQUIRE(TRAP::Math::IsInf(TRAP::Math::Determinant(input1)));
        REQUIRE(TRAP::Math::IsInf(TRAP::Math::Determinant(input2)));
    }
}

TEST_CASE("TRAP::Math::Determinant()", "[math][generic][determinant]")
{
    SECTION("Mat3 - double")
    {
        RunCompileTimeDeterminantTests<TRAP::Math::Mat3d>();
        RunDeterminantEdgeTests<TRAP::Math::Mat3d>();
    }
    SECTION("Mat3 - float")
    {
        RunCompileTimeDeterminantTests<TRAP::Math::Mat3f>();
        RunDeterminantEdgeTests<TRAP::Math::Mat3f>();
    }

    SECTION("Mat4 - double")
    {
        RunCompileTimeDeterminantTests<TRAP::Math::Mat4d>();
        RunDeterminantEdgeTests<TRAP::Math::Mat4d>();
    }
    SECTION("Mat4 - float")
    {
        RunCompileTimeDeterminantTests<TRAP::Math::Mat4f>();
        RunDeterminantEdgeTests<TRAP::Math::Mat4f>();
    }
}
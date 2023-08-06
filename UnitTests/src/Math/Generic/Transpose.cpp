#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>
#include <array>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires ((TRAP::Math::IsMat3<T> || TRAP::Math::IsMat4<T>) && std::floating_point<typename T::valueType>)
consteval void RunCompileTimeTransposeTests()
{
    constexpr typename T::valueType Epsilon = std::numeric_limits<typename T::valueType>::epsilon();

    {
        constexpr T m(TRAP::Math::tMat4<typename T::valueType>(0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f));
        constexpr T t = TRAP::Math::Transpose(m);
        constexpr T expected(TRAP::Math::tMat4<typename T::valueType>(0.0f, 4.0f, 8.0f, 12.0f, 1.0f, 5.0f, 9.0f, 13.0f, 2.0f, 6.0f, 10.0f, 14.0f, 3.0f, 7.0f, 11.0f, 15.0f));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(t, expected, Epsilon)));
    }
    {
        constexpr T m(TRAP::Math::tMat4<typename T::valueType>(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 2.0f, 0.0f, 0.0f, 0.0f, 0.0f, 3.0f, 0.0f, 0.0f, 0.0f, 0.0f, 4.0f));
        constexpr T t = TRAP::Math::Transpose(m);
        static_assert(TRAP::Math::All(TRAP::Math::Equal(t, m, Epsilon)));
    }
    {
        constexpr T m(TRAP::Math::tMat4<typename T::valueType>(0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f));
        constexpr T t = TRAP::Math::Transpose(m);
        constexpr T t2 = TRAP::Math::Transpose(t);
        static_assert(TRAP::Math::All(TRAP::Math::Equal(m, t2, Epsilon)));
    }
}

template<typename T>
requires ((TRAP::Math::IsMat3<T> || TRAP::Math::IsMat4<T>) && std::floating_point<typename T::valueType>)
void RunTransposeEdgeTests()
{
    constexpr typename T::valueType Epsilon = std::numeric_limits<typename T::valueType>::epsilon();

    constexpr typename T::valueType min = std::numeric_limits<typename T::valueType>::lowest();
    constexpr typename T::valueType max = std::numeric_limits<typename T::valueType>::max();
    constexpr typename T::valueType inf = std::numeric_limits<typename T::valueType>::infinity();
    constexpr typename T::valueType ninf = -std::numeric_limits<typename T::valueType>::infinity();
    constexpr typename T::valueType nan = std::numeric_limits<typename T::valueType>::quiet_NaN();

    {
        constexpr T m(TRAP::Math::tMat4<typename T::valueType>(max, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f));
        constexpr T t = TRAP::Math::Transpose(m);
        constexpr T expected(TRAP::Math::tMat4<typename T::valueType>(max, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(t, expected, Epsilon)));
    }
    {
        constexpr T m(TRAP::Math::tMat4<typename T::valueType>(min, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f));
        constexpr T t = TRAP::Math::Transpose(m);
        constexpr T expected(TRAP::Math::tMat4<typename T::valueType>(min, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(t, expected, Epsilon)));
    }
    {
        constexpr T m(TRAP::Math::tMat4<typename T::valueType>(inf, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f));
        constexpr T t = TRAP::Math::Transpose(m);
        constexpr T expected(TRAP::Math::tMat4<typename T::valueType>(inf, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(t, expected)));
    }
    {
        constexpr T m(TRAP::Math::tMat4<typename T::valueType>(ninf, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f));
        constexpr T t = TRAP::Math::Transpose(m);
        constexpr T expected(TRAP::Math::tMat4<typename T::valueType>(ninf, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(t, expected)));
    }
    {
        constexpr T m(TRAP::Math::tMat4<typename T::valueType>(nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan));
        constexpr T t = TRAP::Math::Transpose(m);
        REQUIRE(TRAP::Math::All(TRAP::Math::Not(TRAP::Math::Equal(t, m))));
    }
}

TEST_CASE("TRAP::Math::Transpose()", "[math][generic][transpose]")
{
    SECTION("Mat3 - double")
    {
        RunCompileTimeTransposeTests<TRAP::Math::Mat3d>();
        RunTransposeEdgeTests<TRAP::Math::Mat3d>();
    }
    SECTION("Mat3 - float")
    {
        RunCompileTimeTransposeTests<TRAP::Math::Mat3f>();
        RunTransposeEdgeTests<TRAP::Math::Mat3f>();
    }

    SECTION("Mat4 - double")
    {
        RunCompileTimeTransposeTests<TRAP::Math::Mat4d>();
        RunTransposeEdgeTests<TRAP::Math::Mat4d>();
    }
    SECTION("Mat4 - float")
    {
        RunCompileTimeTransposeTests<TRAP::Math::Mat4f>();
        RunTransposeEdgeTests<TRAP::Math::Mat4f>();
    }
}
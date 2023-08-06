#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>
#include <array>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires (TRAP::Math::IsVec<T> && std::floating_point<typename T::valueType> && T::Length() > 2)
consteval void RunCompileTimeOuterProductTests()
{
    constexpr typename T::valueType Epsilon = std::numeric_limits<typename T::valueType>::epsilon();

    {
        constexpr T v1(TRAP::Math::Vec<4, typename T::valueType>(1.0f, 2.0f, 3.0f, 4.0f));
        constexpr T v2(TRAP::Math::Vec<4, typename T::valueType>(5.0f, 6.0f, 7.0f, 8.0f));
        constexpr auto result = TRAP::Math::OuterProduct(v1, v2);
        constexpr decltype(result) expected(TRAP::Math::tMat4<typename T::valueType>(5.0f, 10.0f, 15.0f, 20.0f, 6.0f, 12.0f, 18.0f, 24.0f, 7.0f, 14.0f, 21.0f, 28.0f, 8.0f, 16.0f, 24.0f, 32.0f));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(result, expected, Epsilon)));
    }
    {
        constexpr T v1(TRAP::Math::Vec<4, typename T::valueType>(1.0f, 0.0f, 0.0f, 0.0f));
        constexpr T v2(TRAP::Math::Vec<4, typename T::valueType>(0.0f, 1.0f, 0.0f, 0.0f));
        constexpr auto result = TRAP::Math::OuterProduct(v1, v2);
        constexpr decltype(result) expected(TRAP::Math::tMat4<typename T::valueType>(0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(result, expected, Epsilon)));
    }
    {
        constexpr T v1(TRAP::Math::Vec<4, typename T::valueType>(2.0f));
        constexpr T v2(TRAP::Math::Vec<4, typename T::valueType>(3.0f));
        constexpr auto result = TRAP::Math::OuterProduct(v1, v2);
        constexpr decltype(result) expected(TRAP::Math::tMat4<typename T::valueType>(6.0f, 6.0f, 6.0f, 6.0f, 6.0f, 6.0f, 6.0f, 6.0f, 6.0f, 6.0f, 6.0f, 6.0f, 6.0f, 6.0f, 6.0f, 6.0f));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(result, expected, Epsilon)));
    }
    {
        constexpr T v1(TRAP::Math::Vec<4, typename T::valueType>(1.0f, 0.0f, -1.0f, 0.0f));
        constexpr T v2(TRAP::Math::Vec<4, typename T::valueType>(0.0f, 1.0f, 0.0f, -1.0f));
        constexpr auto result = TRAP::Math::OuterProduct(v1, v2);
        constexpr decltype(result) expected(TRAP::Math::tMat4<typename T::valueType>(0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(result, expected, Epsilon)));
    }
    {
        constexpr T v1(TRAP::Math::Vec<4, typename T::valueType>(1.0f, 2.0f, 3.0f, 4.0f));
        constexpr T v2(TRAP::Math::Vec<4, typename T::valueType>(0.0f));
        constexpr auto result = TRAP::Math::OuterProduct(v1, v2);
        constexpr decltype(result) expected(TRAP::Math::tMat4<typename T::valueType>(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(result, expected, Epsilon)));
    }
    {
        constexpr T v1(TRAP::Math::Vec<4, typename T::valueType>(1.0f, 2.0f, 3.0f, 4.0f));
        constexpr T v2(TRAP::Math::Vec<4, typename T::valueType>(1.0f, 2.0f, 3.0f, 4.0f));
        constexpr auto result = TRAP::Math::OuterProduct(v1, v2);
        constexpr decltype(result) expected(TRAP::Math::tMat4<typename T::valueType>(1.0f, 2.0f, 3.0f, 4.0f, 2.0f, 4.0f, 6.0f, 8.0f, 3.0f, 6.0f, 9.0f, 12.0f, 4.0f, 8.0f, 12.0f, 16.0f));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(result, expected, Epsilon)));
    }
}

template<typename T>
requires (TRAP::Math::IsVec<T> && std::floating_point<typename T::valueType> && T::Length() > 2)
void RunOuterProductEdgeTests()
{
    constexpr typename T::valueType min = std::numeric_limits<typename T::valueType>::lowest();
    constexpr typename T::valueType max = std::numeric_limits<typename T::valueType>::max();
    constexpr typename T::valueType inf = std::numeric_limits<typename T::valueType>::infinity();
    constexpr typename T::valueType ninf = -std::numeric_limits<typename T::valueType>::infinity();
    constexpr typename T::valueType nan = std::numeric_limits<typename T::valueType>::quiet_NaN();

    {
        constexpr T v1(TRAP::Math::Vec<4, typename T::valueType>{max});
        constexpr T v2(TRAP::Math::Vec<4, typename T::valueType>{min});
        const auto result = TRAP::Math::OuterProduct(v1, v2);
        constexpr decltype(result) expected(TRAP::Math::tMat4<typename T::valueType>(ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(result, expected)));
    }
    {
        constexpr T v(TRAP::Math::Vec<4, typename T::valueType>{inf});
        constexpr auto result = TRAP::Math::OuterProduct(v, v);
        constexpr decltype(result) expected(TRAP::Math::tMat4<typename T::valueType>(inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(result, expected)));
    }
    {
        constexpr T v1(TRAP::Math::Vec<4, typename T::valueType>{inf});
        constexpr T v2(TRAP::Math::Vec<4, typename T::valueType>{ninf});
        constexpr auto result = TRAP::Math::OuterProduct(v1, v2);
        constexpr decltype(result) expected(TRAP::Math::tMat4<typename T::valueType>(ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(result, expected)));
    }
    {
        constexpr T v1(TRAP::Math::Vec<4, typename T::valueType>{nan});
        constexpr T v2(TRAP::Math::Vec<4, typename T::valueType>{1.f});
        const auto result = TRAP::Math::OuterProduct(v1, v2);
        constexpr decltype(result) expected(TRAP::Math::tMat4<typename T::valueType>(nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan));
        REQUIRE(TRAP::Math::All(TRAP::Math::Not(TRAP::Math::Equal(result, expected))));
    }
    {
        constexpr T v(TRAP::Math::Vec<4, typename T::valueType>{nan});
        const auto result = TRAP::Math::OuterProduct(v, v);
        constexpr decltype(result) expected(TRAP::Math::tMat4<typename T::valueType>(nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan));
        REQUIRE(TRAP::Math::All(TRAP::Math::Not(TRAP::Math::Equal(result, expected))));
    }
}

TEST_CASE("TRAP::Math::OuterProduct()", "[math][generic][outerproduct]")
{
    SECTION("Vec3 - double")
    {
        RunCompileTimeOuterProductTests<TRAP::Math::Vec3d>();
        RunOuterProductEdgeTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunCompileTimeOuterProductTests<TRAP::Math::Vec3f>();
        RunOuterProductEdgeTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - double")
    {
        RunCompileTimeOuterProductTests<TRAP::Math::Vec4d>();
        RunOuterProductEdgeTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunCompileTimeOuterProductTests<TRAP::Math::Vec4f>();
        RunOuterProductEdgeTests<TRAP::Math::Vec4f>();
    }
}
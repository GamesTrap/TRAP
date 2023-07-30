#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>
#include <array>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::floating_point<T> || (TRAP::Math::IsVec<T> && std::floating_point<typename T::valueType>)
void RunFrExpTests()
{
    if constexpr(std::floating_point<T>)
    {
        constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        constexpr std::array<std::tuple<T, T, int32_t>, 4> values
        {
            std::tuple(T(1024.0f), T(0.5f), 11),
            std::tuple(T(0.24f), T(0.96f), -2),
            std::tuple(T(0.0f), T(0.0f), 0),
            std::tuple(T(-1.33f), T(-0.665f), 1),
        };

        for(const auto& [x, expectedRes, expectedExp] : values)
        {
            int32_t exp{};
            const T res = TRAP::Math::FrExp(x, exp);
            REQUIRE(TRAP::Math::Equal(res, expectedRes, Epsilon));
            REQUIRE(exp == expectedExp);
        }
    }
    else if constexpr(TRAP::Math::IsVec<T> && std::floating_point<typename T::valueType>)
    {
        constexpr typename T::valueType Epsilon = std::numeric_limits<typename T::valueType>::epsilon();

        constexpr T x(TRAP::Math::Vec<4, typename T::valueType>(1024.0f, 0.24f, 0.0f, -1.33f));
        TRAP::Math::Vec<T::Length(), int32_t> exp{};
        const T res = TRAP::Math::FrExp(x, exp);
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(res, T(TRAP::Math::Vec<4, typename T::valueType>(0.5f, 0.96f, 0.0f, -0.665f)), Epsilon)));
        REQUIRE(exp == TRAP::Math::Vec<T::Length(), int32_t>(TRAP::Math::Vec<4, int32_t>(11, -2, 0, 1)));
    }
}

template<typename T>
requires std::floating_point<T>
void RunFrExpEdgeTests()
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    constexpr T inf = std::numeric_limits<T>::infinity();
    constexpr T ninf = -std::numeric_limits<T>::infinity();
    constexpr T nan = std::numeric_limits<T>::quiet_NaN();
    constexpr T max = std::numeric_limits<T>::max();
    constexpr T min = std::numeric_limits<T>::denorm_min();
    constexpr T nmin = -std::numeric_limits<T>::denorm_min();

    int32_t exp{};
    T res{};

    res = TRAP::Math::FrExp(T(0.0f), exp);
    REQUIRE(TRAP::Math::Equal(res, T(0.0f), Epsilon));
    REQUIRE(exp == 0);

    res = TRAP::Math::FrExp(inf, exp);
    REQUIRE(TRAP::Math::IsInf(res));

    res = TRAP::Math::FrExp(ninf, exp);
    REQUIRE(TRAP::Math::IsInf(res));

    res = TRAP::Math::FrExp(nan, exp);
    REQUIRE(TRAP::Math::IsNaN(res));

    if constexpr(std::same_as<T, double>)
    {
        res = TRAP::Math::FrExp(max, exp);
        REQUIRE(TRAP::Math::Equal(res, T(0.99999999999999989f), Epsilon));
        REQUIRE(exp == 1024);

        res = TRAP::Math::FrExp(min, exp);
        REQUIRE(TRAP::Math::Equal(res, T(0.5f), Epsilon));
        REQUIRE(exp == -1073);

        res = TRAP::Math::FrExp(nmin, exp);
        REQUIRE(TRAP::Math::Equal(res, T(-0.5f), Epsilon));
        REQUIRE(exp == -1073);
    }
    else if constexpr(std::same_as<T, float>)
    {
        res = TRAP::Math::FrExp(max, exp);
        REQUIRE(TRAP::Math::Equal(res, T(0.99999999999999989f), Epsilon));
        REQUIRE(exp == 128);

        res = TRAP::Math::FrExp(min, exp);
        REQUIRE(TRAP::Math::Equal(res, T(0.5f), Epsilon));
        REQUIRE(exp == -148);

        res = TRAP::Math::FrExp(nmin, exp);
        REQUIRE(TRAP::Math::Equal(res, T(-0.5f), Epsilon));
        REQUIRE(exp == -148);
    }
}

TEST_CASE("TRAP::Math::FrExp()", "[math][generic][frexp]")
{
    SECTION("Scalar - double")
    {
        RunFrExpTests<double>();
        RunFrExpEdgeTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunFrExpTests<float>();
        RunFrExpEdgeTests<float>();
    }

    SECTION("Vec2 - double")
    {
        RunFrExpTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunFrExpTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - double")
    {
        RunFrExpTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunFrExpTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - double")
    {
        RunFrExpTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunFrExpTests<TRAP::Math::Vec4f>();
    }
}
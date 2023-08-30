#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>
#include <array>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::floating_point<T> || (TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>)
void RunLdExpTests()
{
    if constexpr(std::floating_point<T>)
    {
        constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        constexpr std::array<std::tuple<T, int32_t, T>, 4> values
        {
            std::tuple(T(0.5f), 11, T(1024.0f)),
            std::tuple(T(0.96f), -2, T(0.24f)),
            std::tuple(T(0.0f), 0, T(0.0f)),
            std::tuple(T(-0.665f), 1, T(-1.33f)),
        };

        for(const auto& [x, exp, expected] : values)
        {
            const T res = TRAP::Math::LdExp(x, exp);
            REQUIRE(TRAP::Math::Equal(res, expected, Epsilon));
        }
    }
    else if constexpr(TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>)
    {
        constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

        constexpr T x(TRAP::Math::Vec<4, typename T::value_type>(0.5f, 0.96f, 0.0f, -0.665f));
        constexpr TRAP::Math::Vec<T::Length(), int32_t> exp(TRAP::Math::Vec<4, int32_t>(11, -2, 0, 1));
        const T res = TRAP::Math::LdExp(x, exp);
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(res, T(TRAP::Math::Vec<4, typename T::value_type>(1024.0f, 0.24f, 0.0f, -1.33f)), Epsilon)));
    }
}

template<typename T>
requires std::floating_point<T>
void RunLdExpEdgeTests()
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    constexpr T inf = std::numeric_limits<T>::infinity();
    constexpr T ninf = -std::numeric_limits<T>::infinity();
    constexpr T nan = std::numeric_limits<T>::quiet_NaN();
    constexpr T max = std::numeric_limits<T>::max();
    constexpr T min = std::numeric_limits<T>::denorm_min();
    constexpr T nmin = -std::numeric_limits<T>::denorm_min();

    T res{};

    res = TRAP::Math::LdExp(T(0.0f), 5);
    REQUIRE(TRAP::Math::Equal(res, T(0.0f), Epsilon));

    res = TRAP::Math::LdExp(inf, 10);
    REQUIRE(TRAP::Math::IsInf(res));

    res = TRAP::Math::LdExp(ninf, 10);
    REQUIRE(TRAP::Math::IsInf(res));

    res = TRAP::Math::LdExp(nan, 0);
    REQUIRE(TRAP::Math::IsNaN(res));

    if constexpr(std::same_as<T, double>)
    {
        res = TRAP::Math::LdExp(max, 1);
        REQUIRE(TRAP::Math::IsInf(res));

        res = TRAP::Math::LdExp(min, 1);
        REQUIRE(TRAP::Math::Equal(res, min * 2, Epsilon));

        res = TRAP::Math::LdExp(nmin, -0);
        REQUIRE(TRAP::Math::Equal(res, nmin, Epsilon));
    }
    else if constexpr(std::same_as<T, float>)
    {
        res = TRAP::Math::LdExp(max, 1);
        REQUIRE(TRAP::Math::IsInf(res));

        res = TRAP::Math::LdExp(min, 1);
        REQUIRE(TRAP::Math::Equal(res, min * 2, Epsilon));

        res = TRAP::Math::LdExp(nmin, -0);
        REQUIRE(TRAP::Math::Equal(res, nmin, Epsilon));
    }
}

TEST_CASE("TRAP::Math::LdExp()", "[math][generic][ldexp]")
{
    SECTION("Scalar - double")
    {
        RunLdExpTests<double>();
        RunLdExpEdgeTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunLdExpTests<float>();
        RunLdExpEdgeTests<float>();
    }

    SECTION("Vec2 - double")
    {
        RunLdExpTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunLdExpTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - double")
    {
        RunLdExpTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunLdExpTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - double")
    {
        RunLdExpTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunLdExpTests<TRAP::Math::Vec4f>();
    }
}
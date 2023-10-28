#include <cstdint>
#include <limits>
#include <array>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires std::floating_point<T> || (TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>)
    void RunFrExpTests()
    {
        if constexpr(std::floating_point<T>)
        {
            static constexpr T Epsilon = std::numeric_limits<T>::epsilon();

            static constexpr std::array<std::tuple<T, T, i32>, 4> values
            {
                std::tuple(T(1024.0f), T(0.5f), 11),
                std::tuple(T(0.24f), T(0.96f), -2),
                std::tuple(T(0.0f), T(0.0f), 0),
                std::tuple(T(-1.33f), T(-0.665f), 1),
            };

            for(const auto& [x, expectedRes, expectedExp] : values)
            {
                i32 exp{};
                const T res = TRAP::Math::FrExp(x, exp);
                REQUIRE(TRAP::Math::Equal(res, expectedRes, Epsilon));
                REQUIRE(exp == expectedExp);
            }
        }
        else if constexpr(TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>)
        {
            static constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

            static constexpr T x(TRAP::Math::Vec<4, typename T::value_type>(1024.0f, 0.24f, 0.0f, -1.33f));
            TRAP::Math::Vec<T::Length(), i32> exp{};
            const T res = TRAP::Math::FrExp(x, exp);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(res, T(TRAP::Math::Vec<4, typename T::value_type>(0.5f, 0.96f, 0.0f, -0.665f)), Epsilon)));
            REQUIRE(exp == TRAP::Math::Vec<T::Length(), i32>(TRAP::Math::Vec<4, i32>(11, -2, 0, 1)));
        }
    }

    template<typename T>
    requires std::floating_point<T>
    void RunFrExpEdgeTests()
    {
        static constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        static constexpr T inf = std::numeric_limits<T>::infinity();
        static constexpr T ninf = -std::numeric_limits<T>::infinity();
        static constexpr T nan = std::numeric_limits<T>::quiet_NaN();
        static constexpr T max = std::numeric_limits<T>::max();
        static constexpr T min = std::numeric_limits<T>::denorm_min();
        static constexpr T nmin = -std::numeric_limits<T>::denorm_min();

        i32 exp{};
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

        if constexpr(std::same_as<T, f64>)
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
        else if constexpr(std::same_as<T, f32>)
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
}

TEST_CASE("TRAP::Math::FrExp()", "[math][generic][frexp]")
{
    SECTION("Scalar - f64")
    {
        RunFrExpTests<f64>();
        RunFrExpEdgeTests<f64>();
    }
    SECTION("Scalar - f32")
    {
        RunFrExpTests<f32>();
        RunFrExpEdgeTests<f32>();
    }

    SECTION("Vec2 - f64")
    {
        RunFrExpTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - f32")
    {
        RunFrExpTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - f64")
    {
        RunFrExpTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - f32")
    {
        RunFrExpTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - f64")
    {
        RunFrExpTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - f32")
    {
        RunFrExpTests<TRAP::Math::Vec4f>();
    }
}
#include <cstdint>
#include <limits>
#include <array>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires std::floating_point<T> || (TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>)
    void RunLdExpTests()
    {
        if constexpr(std::floating_point<T>)
        {
            static constexpr T Epsilon = std::numeric_limits<T>::epsilon();

            static constexpr std::array<std::tuple<T, i32, T>, 4> values
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
            static constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

            static constexpr T x(TRAP::Math::Vec<4, typename T::value_type>(0.5f, 0.96f, 0.0f, -0.665f));
            static constexpr TRAP::Math::Vec<T::Length(), i32> exp(TRAP::Math::Vec<4, i32>(11, -2, 0, 1));
            const T res = TRAP::Math::LdExp(x, exp);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(res, T(TRAP::Math::Vec<4, typename T::value_type>(1024.0f, 0.24f, 0.0f, -1.33f)), Epsilon)));
        }
    }

    template<typename T>
    requires std::floating_point<T>
    void RunLdExpEdgeTests()
    {
        static constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        static constexpr T inf = std::numeric_limits<T>::infinity();
        static constexpr T ninf = -std::numeric_limits<T>::infinity();
        static constexpr T nan = std::numeric_limits<T>::quiet_NaN();
        static constexpr T max = std::numeric_limits<T>::max();
        static constexpr T min = std::numeric_limits<T>::denorm_min();
        static constexpr T nmin = -std::numeric_limits<T>::denorm_min();

        T res{};

        res = TRAP::Math::LdExp(T(0.0f), 5);
        REQUIRE(TRAP::Math::Equal(res, T(0.0f), Epsilon));

        res = TRAP::Math::LdExp(inf, 10);
        REQUIRE(TRAP::Math::IsInf(res));

        res = TRAP::Math::LdExp(ninf, 10);
        REQUIRE(TRAP::Math::IsInf(res));

        res = TRAP::Math::LdExp(nan, 0);
        REQUIRE(TRAP::Math::IsNaN(res));

        if constexpr(std::same_as<T, f64>)
        {
            res = TRAP::Math::LdExp(max, 1);
            REQUIRE(TRAP::Math::IsInf(res));

            res = TRAP::Math::LdExp(min, 1);
            REQUIRE(TRAP::Math::Equal(res, min * 2, Epsilon));

            res = TRAP::Math::LdExp(nmin, -0);
            REQUIRE(TRAP::Math::Equal(res, nmin, Epsilon));
        }
        else if constexpr(std::same_as<T, f32>)
        {
            res = TRAP::Math::LdExp(max, 1);
            REQUIRE(TRAP::Math::IsInf(res));

            res = TRAP::Math::LdExp(min, 1);
            REQUIRE(TRAP::Math::Equal(res, min * 2, Epsilon));

            res = TRAP::Math::LdExp(nmin, -0);
            REQUIRE(TRAP::Math::Equal(res, nmin, Epsilon));
        }
    }
}

TEST_CASE("TRAP::Math::LdExp()", "[math][generic][ldexp]")
{
    SECTION("Scalar - f64")
    {
        RunLdExpTests<f64>();
        RunLdExpEdgeTests<f64>();
    }
    SECTION("Scalar - f32")
    {
        RunLdExpTests<f32>();
        RunLdExpEdgeTests<f32>();
    }

    SECTION("Vec2 - f64")
    {
        RunLdExpTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - f32")
    {
        RunLdExpTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - f64")
    {
        RunLdExpTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - f32")
    {
        RunLdExpTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - f64")
    {
        RunLdExpTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - f32")
    {
        RunLdExpTests<TRAP::Math::Vec4f>();
    }
}
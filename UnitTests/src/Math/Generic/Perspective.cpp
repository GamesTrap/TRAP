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
    requires std::floating_point<T>
    consteval void RunPerspectiveCompileTimeTests()
    {
        constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        {
            constexpr auto p = TRAP::Math::Perspective<T>(TRAP::Math::Radians<T>(45.0f), T(4.0f) / T(3.0f), 0.1f, 100.0f);
            constexpr TRAP::Math::tMat4<T> expected(1.810660f, 0.0f, 0.0f, 0.0f, 0.0f, 2.414213f, 0.0f, 0.0f, 0.0f, 0.0f, -1.001001f, -1.0f, 0.0f, 0.0f, -0.100100f, 0.0f);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(p, expected, T(0.000001f))));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(p, TRAP::Math::PerspectiveReverseZ<T>(TRAP::Math::Radians<T>(45.0f), T(4.0f) / T(3.0f), 100.0f, 0.1f), Epsilon)));
        }
        {
            constexpr auto p = TRAP::Math::Perspective<T>(TRAP::Math::Radians<T>(120.0f), T(1.0f), 1.0f, 1000.0f);
            constexpr TRAP::Math::tMat4<T> expected(0.577350f, 0.0f, 0.0f, 0.0f, 0.0f, 0.577350f, 0.0f, 0.0f, 0.0f, 0.0f, -1.001001f, -1.0f, 0.0f, 0.0f, -1.001001f, 0.0f);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(p, expected, T(0.000001f))));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(p, TRAP::Math::PerspectiveReverseZ<T>(TRAP::Math::Radians<T>(120.0f), T(1.0f), 1000.0f, 1.0f), Epsilon)));
        }
        {
            constexpr auto p = TRAP::Math::Perspective<T>(TRAP::Math::Radians<T>(30.0f), T(16.0f) / T(9.0f), 0.01f, 100.0f);
            constexpr TRAP::Math::tMat4<T> expected(2.099278f, 0.0f, 0.0f, 0.0f, 0.0f, 3.732051f, 0.0f, 0.0f, 0.0f, 0.0f, -1.000100f, -1.0f, 0.0f, 0.0f, -0.010001f, 0.0f);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(p, expected, T(0.000001f))));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(p, TRAP::Math::PerspectiveReverseZ<T>(TRAP::Math::Radians<T>(30.0f), T(16.0f) / T(9.0f), 100.0f, 0.01f), Epsilon)));
        }
        {
            constexpr auto p = TRAP::Math::Perspective<T>(TRAP::Math::Radians<T>(-60.0f), T(2.0f), 0.1f, 100.0f);
            constexpr TRAP::Math::tMat4<T> expected(-0.866025f, 0.0f, 0.0f, 0.0f, 0.0f, -1.732051f, 0.0f, 0.0f, 0.0f, 0.0f, -1.001001f, -1.0f, 0.0f, 0.0f, -0.100100f, 0.0f);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(p, expected, T(0.000001f))));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(p, TRAP::Math::PerspectiveReverseZ<T>(TRAP::Math::Radians<T>(-60.0f), T(2.0f), 100.0f, 0.1f), Epsilon)));
        }
        {
            constexpr auto p = TRAP::Math::Perspective<T>(TRAP::Math::Radians<T>(90.0f), T(1.0f), 0.0f, 100.0f);
            constexpr TRAP::Math::tMat4<T> expected(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(p, expected, Epsilon)));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(p, TRAP::Math::PerspectiveReverseZ<T>(TRAP::Math::Radians<T>(90.0f), T(1.0f), 100.0f, 0.0f), Epsilon)));
        }
        {
            constexpr auto p = TRAP::Math::Perspective<T>(TRAP::Math::Radians<T>(60.0f), T(0.0001f), 10.0f, 1000.0f);
            constexpr TRAP::Math::tMat4<T> expected(17320.507812f, 0.0f, 0.0f, 0.0f, 0.0f, 1.732051f, 0.0f, 0.0f, 0.0f, 0.0f, -1.010101f, -1.0f, 0.0f, 0.0f, -10.101010f, 0.0f);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(p, expected, T(0.001f))));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(p, TRAP::Math::PerspectiveReverseZ<T>(TRAP::Math::Radians<T>(60.0f), T(0.0001f), 1000.0f, 10.0f), Epsilon)));
        }
        {
            constexpr auto p = TRAP::Math::Perspective<T>(TRAP::Math::Radians<T>(45.0f), T(1.0f), 0.001f, 0.01f);
            constexpr TRAP::Math::tMat4<T> expected(2.414213f, 0.0f, 0.0f, 0.0f, 0.0f, 2.414213f, 0.0f, 0.0f, 0.0f, 0.0f, -1.111111f, -1.0f, 0.0f, 0.0f, -0.001111f, 0.0f);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(p, expected, T(0.000001f))));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(p, TRAP::Math::PerspectiveReverseZ<T>(TRAP::Math::Radians<T>(45.0f), T(1.0f), 0.01f, 0.001f), Epsilon)));
        }
    }

    template<typename T>
    requires std::floating_point<T>
    void RunPerspectiveRunTimeTests()
    {
        static constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        {
            const auto p = TRAP::Math::Perspective<T>(TRAP::Math::Radians<T>(45.0f), T(4.0f) / T(3.0f), 0.1f, 100.0f);
            static constexpr TRAP::Math::tMat4<T> expected(1.810660f, 0.0f, 0.0f, 0.0f, 0.0f, 2.414213f, 0.0f, 0.0f, 0.0f, 0.0f, -1.001001f, -1.0f, 0.0f, 0.0f, -0.100100f, 0.0f);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(p, expected, T(0.000001f))));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(p, TRAP::Math::PerspectiveReverseZ<T>(TRAP::Math::Radians<T>(45.0f), T(4.0f) / T(3.0f), 100.0f, 0.1f), Epsilon)));
        }
        {
            const auto p = TRAP::Math::Perspective<T>(TRAP::Math::Radians<T>(120.0f), T(1.0f), 1.0f, 1000.0f);
            static constexpr TRAP::Math::tMat4<T> expected(0.577350f, 0.0f, 0.0f, 0.0f, 0.0f, 0.577350f, 0.0f, 0.0f, 0.0f, 0.0f, -1.001001f, -1.0f, 0.0f, 0.0f, -1.001001f, 0.0f);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(p, expected, T(0.000001f))));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(p, TRAP::Math::PerspectiveReverseZ<T>(TRAP::Math::Radians<T>(120.0f), T(1.0f), 1000.0f, 1.0f), Epsilon)));
        }
        {
            const auto p = TRAP::Math::Perspective<T>(TRAP::Math::Radians<T>(30.0f), T(16.0f) / T(9.0f), 0.01f, 100.0f);
            static constexpr TRAP::Math::tMat4<T> expected(2.099278f, 0.0f, 0.0f, 0.0f, 0.0f, 3.732051f, 0.0f, 0.0f, 0.0f, 0.0f, -1.000100f, -1.0f, 0.0f, 0.0f, -0.010001f, 0.0f);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(p, expected, T(0.000001f))));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(p, TRAP::Math::PerspectiveReverseZ<T>(TRAP::Math::Radians<T>(30.0f), T(16.0f) / T(9.0f), 100.0f, 0.01f), Epsilon)));
        }
        {
            const auto p = TRAP::Math::Perspective<T>(TRAP::Math::Radians<T>(-60.0f), T(2.0f), 0.1f, 100.0f);
            static constexpr TRAP::Math::tMat4<T> expected(-0.866025f, 0.0f, 0.0f, 0.0f, 0.0f, -1.732051f, 0.0f, 0.0f, 0.0f, 0.0f, -1.001001f, -1.0f, 0.0f, 0.0f, -0.100100f, 0.0f);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(p, expected, T(0.000001f))));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(p, TRAP::Math::PerspectiveReverseZ<T>(TRAP::Math::Radians<T>(-60.0f), T(2.0f), 100.0f, 0.1f), T(0.000001f))));
        }
        {
            const auto p = TRAP::Math::Perspective<T>(TRAP::Math::Radians<T>(90.0f), T(1.0f), 0.0f, 100.0f);
            static constexpr TRAP::Math::tMat4<T> expected(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(p, expected, Epsilon)));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(p, TRAP::Math::PerspectiveReverseZ<T>(TRAP::Math::Radians<T>(90.0f), T(1.0f), 100.0f, 0.0f), Epsilon)));
        }
        {
            const auto p = TRAP::Math::Perspective<T>(TRAP::Math::Radians<T>(60.0f), T(0.0001f), 10.0f, 1000.0f);
            static constexpr TRAP::Math::tMat4<T> expected(17320.507812f, 0.0f, 0.0f, 0.0f, 0.0f, 1.732051f, 0.0f, 0.0f, 0.0f, 0.0f, -1.010101f, -1.0f, 0.0f, 0.0f, -10.101010f, 0.0f);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(p, expected, T(0.001f))));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(p, TRAP::Math::PerspectiveReverseZ<T>(TRAP::Math::Radians<T>(60.0f), T(0.0001f), 1000.0f, 10.0f), T(0.000001f))));
        }
        {
            const auto p = TRAP::Math::Perspective<T>(TRAP::Math::Radians<T>(45.0f), T(1.0f), 0.001f, 0.01f);
            static constexpr TRAP::Math::tMat4<T> expected(2.414213f, 0.0f, 0.0f, 0.0f, 0.0f, 2.414213f, 0.0f, 0.0f, 0.0f, 0.0f, -1.111111f, -1.0f, 0.0f, 0.0f, -0.001111f, 0.0f);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(p, expected, T(0.000001f))));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(p, TRAP::Math::PerspectiveReverseZ<T>(TRAP::Math::Radians<T>(45.0f), T(1.0f), 0.01f, 0.001f), Epsilon)));
        }
    }

    template<typename T>
    requires std::floating_point<T>
    void RunPerspectiveEdgeTests()
    {
        static constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        static constexpr T min = std::numeric_limits<T>::lowest();
        static constexpr T max = std::numeric_limits<T>::max();
        static constexpr T inf = std::numeric_limits<T>::infinity();
        static constexpr T ninf = -std::numeric_limits<T>::infinity();
        static constexpr T nan = std::numeric_limits<T>::quiet_NaN();

        {
            const auto p = TRAP::Math::Perspective<T>(0.0f, T(2.0f), 1.0f, 100.0f);
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsInf(std::get<0>(p))));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(p, TRAP::Math::PerspectiveReverseZ<T>(0.0f, T(2.0f), 100.0f, 1.0f))));
        }
        {
            const auto p = TRAP::Math::Perspective<T>(min, 1.0f, 1.0f, 100.0f);
            if constexpr(std::same_as<T, float>)
            {
                static constexpr TRAP::Math::tMat4<T> expected(3.550689f, 0.0f, 0.0f, 0.0f, 0.0f, 3.550689f, 0.0f, 0.0f, 0.0f, 0.0f, -1.010101f, -1.0f, 0.0f, 0.0f, -1.010101f, 0.0f);
                REQUIRE(TRAP::Math::All(TRAP::Math::Equal(p, expected, T(0.000001f))));
            }
            else if constexpr(std::same_as<T, double>)
            {
                static constexpr TRAP::Math::tMat4<T> expected(-0.002481f, 0.0f, 0.0f, 0.0f, 0.0f, -0.002481f, 0.0f, 0.0f, 0.0f, 0.0f, -1.010101f, -1.0f, 0.0f, 0.0f, -1.010101f, 0.0f);
                REQUIRE(TRAP::Math::All(TRAP::Math::Equal(p, expected, T(0.000001f))));
            }
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(p, TRAP::Math::PerspectiveReverseZ<T>(min, 1.0f, 100.0f, 1.0f), Epsilon)));
        }
        {
            const auto p = TRAP::Math::Perspective<T>(max, 1.0f, 1.0f, 100.0f);
            if constexpr(std::same_as<T, float>)
            {
                static constexpr TRAP::Math::tMat4<T> expected(-3.550689f, 0.0f, 0.0f, 0.0f, 0.0f, -3.550689f, 0.0f, 0.0f, 0.0f, 0.0f, -1.010101f, -1.0f, 0.0f, 0.0f, -1.010101f, 0.0f);
                REQUIRE(TRAP::Math::All(TRAP::Math::Equal(p, expected, T(0.000001f))));
            }
            else if constexpr(std::same_as<T, double>)
            {
                static constexpr TRAP::Math::tMat4<T> expected(0.002481f, 0.0f, 0.0f, 0.0f, 0.0f, 0.002481f, 0.0f, 0.0f, 0.0f, 0.0f, -1.010101f, -1.0f, 0.0f, 0.0f, -1.010101f, 0.0f);
                REQUIRE(TRAP::Math::All(TRAP::Math::Equal(p, expected, T(0.000001f))));
            }
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(p, TRAP::Math::PerspectiveReverseZ<T>(max, 1.0f, 100.0f, 1.0f), Epsilon)));
        }
        {
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<0>(TRAP::Math::Perspective<T>(inf, 1.0f, 1.0f, 100.0f)))));
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<0>(TRAP::Math::PerspectiveReverseZ<T>(inf, 1.0f, 100.0f, 1.0f)))));
        }
        {
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<0>(TRAP::Math::Perspective<T>(ninf, 1.0f, 1.0f, 100.0f)))));
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<0>(TRAP::Math::PerspectiveReverseZ<T>(ninf, 1.0f, 100.0f, 1.0f)))));
        }
        {
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<0>(TRAP::Math::Perspective<T>(nan, 1.0f, 1.0f, 100.0f)))));
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<0>(TRAP::Math::PerspectiveReverseZ<T>(nan, 1.0f, 100.0f, 1.0f)))));
        }
    }
}

TEST_CASE("TRAP::Math::Perspective()", "[math][generic][perspective]")
{
    SECTION("Mat4 - double")
    {
        RunPerspectiveRunTimeTests<double>();
        RunPerspectiveCompileTimeTests<double>();
        RunPerspectiveEdgeTests<double>();
    }
    SECTION("Mat4 - float")
    {
        RunPerspectiveRunTimeTests<float>();
        RunPerspectiveCompileTimeTests<float>();
        RunPerspectiveEdgeTests<float>();
    }
}
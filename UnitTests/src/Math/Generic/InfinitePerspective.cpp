#include <limits>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires std::floating_point<T>
    consteval void RunInfinitePerspectiveReverseZCompileTimeTests()
    {
        {
            constexpr auto p = TRAP::Math::InfinitePerspectiveReverseZ<T>(TRAP::Math::Radians<T>(45.0f), 1.0f, 0.1f);
            constexpr TRAP::Math::tMat4<T> expected(2.414213f, 0.0f, 0.0f, 0.0f, 0.0f, 2.414213f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.1f, 0.0f);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(p, expected, T(0.000001f))));
        }
        {
            constexpr auto p = TRAP::Math::InfinitePerspectiveReverseZ<T>(TRAP::Math::Radians<T>(90.0f), 16.0f / 9.0f, 0.01f);
            constexpr TRAP::Math::tMat4<T> expected(0.562500f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.01f, 0.0f);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(p, expected, T(0.000001f))));
        }
        {
            constexpr auto p = TRAP::Math::InfinitePerspectiveReverseZ<T>(TRAP::Math::Radians<T>(30.0f), 2.0f, 1.0f);
            constexpr TRAP::Math::tMat4<T> expected(1.866025f, 0.0f, 0.0f, 0.0f, 0.0f, 3.732051f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(p, expected, T(0.000001f))));
        }
    }

    template<typename T>
    requires std::floating_point<T>
    void RunInfinitePerspectiveReverseZRunTimeTests()
    {
        {
            const auto p = TRAP::Math::InfinitePerspectiveReverseZ<T>(TRAP::Math::Radians<T>(45.0f), 1.0f, 0.1f);
            static constexpr TRAP::Math::tMat4<T> expected(2.414213f, 0.0f, 0.0f, 0.0f, 0.0f, 2.414213f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.1f, 0.0f);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(p, expected, T(0.000001f))));
        }
        {
            const auto p = TRAP::Math::InfinitePerspectiveReverseZ<T>(TRAP::Math::Radians<T>(90.0f), 16.0f / 9.0f, 0.01f);
            static constexpr TRAP::Math::tMat4<T> expected(0.562500f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.01f, 0.0f);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(p, expected, T(0.000001f))));
        }
        {
            const auto p = TRAP::Math::InfinitePerspectiveReverseZ<T>(TRAP::Math::Radians<T>(30.0f), 2.0f, 1.0f);
            static constexpr TRAP::Math::tMat4<T> expected(1.866025f, 0.0f, 0.0f, 0.0f, 0.0f, 3.732051f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(p, expected, T(0.000001f))));
        }
    }

    template<typename T>
    requires std::floating_point<T>
    void RunInfinitePerspectiveReverseZEdgeTests()
    {
        static constexpr T min = std::numeric_limits<T>::lowest();
        static constexpr T max = std::numeric_limits<T>::max();
        static constexpr T inf = std::numeric_limits<T>::infinity();
        static constexpr T ninf = -std::numeric_limits<T>::infinity();
        static constexpr T nan = std::numeric_limits<T>::quiet_NaN();

        {
            // const auto p = TRAP::Math::InfinitePerspectiveReverseZ<T>(min, min, min);
            // REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<0>(p))));
            // REQUIRE(TRAP::Math::Any(TRAP::Math::IsInf(std::get<3>(p))));
        }
        {
            // const auto p = TRAP::Math::InfinitePerspectiveReverseZ<T>(max, max, max);
            // REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<0>(p))));
            // REQUIRE(TRAP::Math::Any(TRAP::Math::IsInf(std::get<3>(p))));
        }
        {
            const auto p = TRAP::Math::InfinitePerspectiveReverseZ<T>(inf, 1.0f, 0.1f);
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<0>(p))));
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<1>(p))));
        }
        {
            const auto p = TRAP::Math::InfinitePerspectiveReverseZ<T>(ninf, 1.0f, 0.1f);
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<0>(p))));
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<1>(p))));
        }
        {
            // const auto p = TRAP::Math::InfinitePerspectiveReverseZ<T>(TRAP::Math::Radians(90.0f), 1.0f, nan);
            // REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<0>(p))));
            // REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<1>(p))));
        }
    }
}

TEST_CASE("TRAP::Math::InfinitePerspectiveReverseZ()", "[math][generic][infiniteperspectivereversez]")
{
    SECTION("Mat4 - f64")
    {
        RunInfinitePerspectiveReverseZRunTimeTests<f64>();
        RunInfinitePerspectiveReverseZCompileTimeTests<f64>();
        RunInfinitePerspectiveReverseZEdgeTests<f64>();
    }
    SECTION("Mat4 - f32")
    {
        RunInfinitePerspectiveReverseZRunTimeTests<f32>();
        RunInfinitePerspectiveReverseZCompileTimeTests<f32>();
        RunInfinitePerspectiveReverseZEdgeTests<f32>();
    }
}
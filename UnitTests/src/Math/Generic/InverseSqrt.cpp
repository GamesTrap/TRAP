#include <limits>
#include <array>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires std::floating_point<T>
    consteval void RunInverseSqrtCompileTimeTests()
    {
        static_assert(TRAP::Math::Equal(TRAP::Math::InverseSqrt(T(0.5f)), T(1.41421f), T(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::InverseSqrt(T(1.5f)), T(0.816497f), T(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::InverseSqrt(T(2.0f)), T(0.707107f), T(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::InverseSqrt(T(41.5f)), T(0.15523f), T(0.00001f)));
    }

    template<typename T>
    requires std::floating_point<T>
    void RunInverseSqrtRunTimeTests()
    {
        static constexpr std::array<std::tuple<T, T>, 4> values
        {
            std::tuple(T(0.5f), T(1.41421f)),
            std::tuple(T(1.5f), T(0.816497f)),
            std::tuple(T(2.0f), T(0.707107f)),
            std::tuple(T(41.5f), T(0.15523f))
        };

        for(const auto& [x, expected] : values)
        {
            REQUIRE(TRAP::Math::Equal(TRAP::Math::InverseSqrt(x), expected, T(0.00001f)));
        }
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    void RunInverseSqrtVecCompileTimeTests()
    {
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::InverseSqrt(T(16.0f)) * TRAP::Math::Sqrt(T(16.0f)), T(1.0f), T(0.01f))));
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    void RunInverseSqrtVecRunTimeTests()
    {
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::InverseSqrt(T(16.0f)) * TRAP::Math::Sqrt(T(16.0f)), T(1.0f), T(0.01f))));
    }

    template<typename T>
    requires std::floating_point<T>
    void RunInverseSqrtEdgeTests()
    {
        static constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        static constexpr T nan = std::numeric_limits<T>::quiet_NaN();
        static constexpr T inf = std::numeric_limits<T>::infinity();
        static constexpr T ninf = -std::numeric_limits<T>::infinity();
        static constexpr T min = std::numeric_limits<T>::lowest();

        REQUIRE(TRAP::Math::IsInf(TRAP::Math::InverseSqrt(0.0f)));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::InverseSqrt(-1.0f)));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::InverseSqrt(min)));
        REQUIRE(TRAP::Math::Equal(TRAP::Math::InverseSqrt(inf), T(0.0f), Epsilon));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::InverseSqrt(ninf)));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::InverseSqrt(nan)));
    }
}

TEST_CASE("TRAP::Math::InverseSqrt()", "[math][generic][inversesqrt]")
{
    SECTION("Scalar - f64")
    {
        RunInverseSqrtRunTimeTests<f64>();
        RunInverseSqrtCompileTimeTests<f64>();
        RunInverseSqrtEdgeTests<f64>();
    }
    SECTION("Scalar - f32")
    {
        RunInverseSqrtRunTimeTests<f32>();
        RunInverseSqrtCompileTimeTests<f32>();
        RunInverseSqrtEdgeTests<f32>();
    }

    SECTION("Vec2 - f64")
    {
        RunInverseSqrtVecRunTimeTests<TRAP::Math::Vec2d>();
        RunInverseSqrtVecCompileTimeTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - f32")
    {
        RunInverseSqrtVecRunTimeTests<TRAP::Math::Vec2f>();
        RunInverseSqrtVecCompileTimeTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - f64")
    {
        RunInverseSqrtVecRunTimeTests<TRAP::Math::Vec3d>();
        RunInverseSqrtVecCompileTimeTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - f32")
    {
        RunInverseSqrtVecRunTimeTests<TRAP::Math::Vec3f>();
        RunInverseSqrtVecCompileTimeTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - f64")
    {
        RunInverseSqrtVecRunTimeTests<TRAP::Math::Vec4d>();
        RunInverseSqrtVecCompileTimeTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - f32")
    {
        RunInverseSqrtVecRunTimeTests<TRAP::Math::Vec4f>();
        RunInverseSqrtVecCompileTimeTests<TRAP::Math::Vec4f>();
    }
}
#include <cmath>
#include <limits>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires std::floating_point<T>
    consteval void RunFloorCompileTimeTests()
    {
        constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        static_assert(TRAP::Math::Equal(TRAP::Math::Floor(T(   0.0f)), T(     0.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Floor(T(-  0.0f)), T(     0.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Floor(T(   0.1f)), T(     0.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Floor(T(-  0.1f)), T(-    1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Floor(T(   0.5f)), T(     0.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Floor(T(-  0.5f)), T(-    1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Floor(T(   0.9f)), T(     0.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Floor(T(-  0.9f)), T(-    1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Floor(T(   1.0f)), T(     1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Floor(T(-  1.0f)), T(-    1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Floor(T(   1.1f)), T(     1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Floor(T(   1.5f)), T(     1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Floor(T(-  1.5f)), T(-    2.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Floor(T(   1.9f)), T(     1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Floor(T(-  1.9f)), T(-    2.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Floor(T(   4.2f)), T(     4.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Floor(T(   4.5f)), T(     4.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Floor(T(   4.7f)), T(     4.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Floor(T(   5.0f)), T(     5.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Floor(T(-  4.2f)), T(-    5.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Floor(T(-  4.5f)), T(-    5.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Floor(T(-  4.7f)), T(-    5.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Floor(T(-  5.0f)), T(-    5.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Floor(T( 42e32f)), T( 4.2e+33f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Floor(T(-42e32f)), T(-4.2e+33f), Epsilon));
    }

    template<typename T>
    requires (TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>)
    consteval void RunFloorVecCompileTimeTests()
    {
        constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Floor(T(   0.0f)), T(     0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Floor(T(-  0.0f)), T(     0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Floor(T(   0.1f)), T(     0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Floor(T(-  0.1f)), T(-    1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Floor(T(   0.5f)), T(     0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Floor(T(-  0.5f)), T(-    1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Floor(T(   0.9f)), T(     0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Floor(T(-  0.9f)), T(-    1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Floor(T(   1.0f)), T(     1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Floor(T(-  1.0f)), T(-    1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Floor(T(   1.1f)), T(     1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Floor(T(   1.5f)), T(     1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Floor(T(-  1.5f)), T(-    2.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Floor(T(   1.9f)), T(     1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Floor(T(-  1.9f)), T(-    2.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Floor(T(   4.2f)), T(     4.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Floor(T(   4.5f)), T(     4.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Floor(T(   4.7f)), T(     4.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Floor(T(   5.0f)), T(     5.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Floor(T(-  4.2f)), T(-    5.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Floor(T(-  4.5f)), T(-    5.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Floor(T(-  4.7f)), T(-    5.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Floor(T(-  5.0f)), T(-    5.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Floor(T( 42e32f)), T( 4.2e+33f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Floor(T(-42e32f)), T(-4.2e+33f), Epsilon)));
    }

    template<typename T>
    requires std::floating_point<T> || (TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>)
    void RunFloorRunTimeTests()
    {
        static constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        static constexpr std::array<T, 25> values
        {
            T(0.0f), T(-0.0f), T(0.1f), T(-0.1f), T(0.5f), T(-0.5f), T(0.9f), T(-0.9f), T(1.0f), T(-1.0f), T(1.1f),
            T(1.5f), T(-1.5f), T(1.9f), T(-1.9f), T(4.2f), T(4.5f), T(4.7f), T(5.0f), T(-4.2f), T(-4.5f),
            T(-4.7f), T(-5.0f), T(42e32f), T(-42e32f)
        };

        for(const T val : values)
        {
            if constexpr(std::floating_point<T>)
            {
                REQUIRE(TRAP::Math::Equal(TRAP::Math::Floor(val), std::floor(val), Epsilon));
            }
            else if constexpr(TRAP::Math::IsVec<T>)
            {
                REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Floor(val), T(std::floor(std::get<0>(val))), Epsilon)));
            }
        }
    }

    template<typename T>
    requires std::floating_point<T>
    consteval void RunFloorEdgeCompileTimeTests()
    {
        constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        constexpr T max = std::numeric_limits<T>::max();
        static_assert(TRAP::Math::Equal(TRAP::Math::Floor(max), max, Epsilon));
        constexpr T min = std::numeric_limits<T>::lowest();
        static_assert(TRAP::Math::Equal(TRAP::Math::Floor(min), min, Epsilon));
        constexpr T nan = std::numeric_limits<T>::quiet_NaN();
        static_assert(TRAP::Math::IsNaN(TRAP::Math::Floor(nan)));
        constexpr T inf = std::numeric_limits<T>::infinity();
        static_assert(TRAP::Math::Equal(TRAP::Math::Floor(inf), inf));
        constexpr T ninf = -std::numeric_limits<T>::infinity();
        static_assert(TRAP::Math::Equal(TRAP::Math::Floor(ninf), ninf));
    }

    template<typename T>
    requires std::floating_point<T>
    void RunFloorEdgeRunTimeTests()
    {
        static constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        static constexpr T max = std::numeric_limits<T>::max();
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Floor(max), max, Epsilon));
        static constexpr T min = std::numeric_limits<T>::lowest();
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Floor(min), min, Epsilon));
        static constexpr T nan = std::numeric_limits<T>::quiet_NaN();
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Floor(nan)));
        static constexpr T inf = std::numeric_limits<T>::infinity();
        REQUIRE(TRAP::Math::IsInf(TRAP::Math::Floor(inf)));
        static constexpr T ninf = -std::numeric_limits<T>::infinity();
        REQUIRE(TRAP::Math::IsInf(TRAP::Math::Floor(ninf)));
    }
}

TEST_CASE("TRAP::Math::Floor()", "[math][generic][floor]")
{
    SECTION("Scalar - f64")
    {
        RunFloorRunTimeTests<f64>();
        RunFloorCompileTimeTests<f64>();
        RunFloorEdgeCompileTimeTests<f64>();
        RunFloorEdgeRunTimeTests<f64>();
    }
    SECTION("Scalar - f32")
    {
        RunFloorRunTimeTests<f32>();
        RunFloorCompileTimeTests<f32>();
        RunFloorEdgeCompileTimeTests<f32>();
        RunFloorEdgeRunTimeTests<f32>();
    }

    SECTION("Vec2 - f64")
    {
        RunFloorRunTimeTests<TRAP::Math::Vec2d>();
        RunFloorVecCompileTimeTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - f32")
    {
        RunFloorRunTimeTests<TRAP::Math::Vec2f>();
        RunFloorVecCompileTimeTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - f64")
    {
        RunFloorRunTimeTests<TRAP::Math::Vec3d>();
        RunFloorVecCompileTimeTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - f32")
    {
        RunFloorRunTimeTests<TRAP::Math::Vec3f>();
        RunFloorVecCompileTimeTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - f64")
    {
        RunFloorRunTimeTests<TRAP::Math::Vec4d>();
        RunFloorVecCompileTimeTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - f32")
    {
        RunFloorRunTimeTests<TRAP::Math::Vec4f>();
        RunFloorVecCompileTimeTests<TRAP::Math::Vec4f>();
    }
}
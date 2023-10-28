#include <cmath>
#include <limits>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires std::floating_point<T>
    consteval void RunRoundCompileTimeTests()
    {
        constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        static_assert(TRAP::Math::Equal(TRAP::Math::Round(T(   0.0f)), T( 0.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Round(T(-  0.0f)), T( 0.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Round(T(   0.1f)), T( 0.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Round(T(-  0.1f)), T( 0.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Round(T(   0.5f)), T( 1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Round(T(-  0.5f)), T(-1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Round(T(   0.9f)), T( 1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Round(T(-  0.9f)), T(-1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Round(T(   1.0f)), T( 1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Round(T(-  1.0f)), T(-1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Round(T(   1.5f)), T( 2.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Round(T(-  1.5f)), T(-2.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Round(T(   1.9f)), T( 2.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Round(T(-  1.9f)), T(-2.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Round(T(   4.2f)), T( 4.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Round(T(   4.5f)), T( 5.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Round(T(   4.7f)), T( 5.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Round(T(   5.0f)), T( 5.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Round(T(-  4.2f)), T(-4.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Round(T(-  4.5f)), T(-5.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Round(T(-  4.7f)), T(-5.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Round(T(-  5.0f)), T(-5.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Round(T( 42e32f)), T( 4.2e+33f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Round(T(-42e32f)), T(-4.2e+33f), Epsilon));
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    consteval void RunRoundVecCompileTimeTests()
    {
        constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Round(T(   0.0f)), T( 0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Round(T(-  0.0f)), T( 0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Round(T(   0.1f)), T( 0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Round(T(-  0.1f)), T( 0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Round(T(   0.5f)), T( 1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Round(T(-  0.5f)), T(-1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Round(T(   0.9f)), T( 1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Round(T(-  0.9f)), T(-1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Round(T(   1.0f)), T( 1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Round(T(-  1.0f)), T(-1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Round(T(   1.5f)), T( 2.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Round(T(-  1.5f)), T(-2.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Round(T(   1.9f)), T( 2.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Round(T(-  1.9f)), T(-2.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Round(T(   4.2f)), T( 4.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Round(T(   4.5f)), T( 5.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Round(T(   4.7f)), T( 5.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Round(T(   5.0f)), T( 5.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Round(T(-  4.2f)), T(-4.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Round(T(-  4.5f)), T(-5.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Round(T(-  4.7f)), T(-5.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Round(T(-  5.0f)), T(-5.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Round(T( 42e32f)), T( 4.2e+33f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Round(T(-42e32f)), T(-4.2e+33f), Epsilon)));
    }

    template<typename T>
    requires std::floating_point<T> || (TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>)
    void RunRoundRunTimeTests()
    {
        static constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        static constexpr std::array<T, 24> values
        {
            T(0.0f), T(-0.0f), T(0.1f), T(-0.1f), T(0.5f), T(-0.5f), T(0.9f), T(-0.9f), T(1.0f), T(-1.0f),
            T(1.5f), T(-1.5f), T(1.9f), T(-1.9f), T(4.2f), T(4.5f), T(4.7f), T(5.0f), T(-4.2f), T(-4.5f),
            T(-4.7f), T(-5.0f), T(42e32f), T(-42e32f)
        };

        for(auto& val : values)
        {
            if constexpr(std::floating_point<T>)
            {
                REQUIRE(TRAP::Math::Equal(TRAP::Math::Round(val), T(std::round(val)), Epsilon));
            }
            else if constexpr(TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>)
            {
                REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Round(val), T(std::round(std::get<0>(val))), Epsilon)));
            }
        }
    }

    template<typename T>
    requires std::floating_point<T>
    consteval void RunRoundEdgeCompileTimeTests()
    {
        constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        constexpr T max = std::numeric_limits<T>::max();
        constexpr T min = std::numeric_limits<T>::lowest();
        constexpr T nan = std::numeric_limits<T>::quiet_NaN();
        constexpr T inf = std::numeric_limits<T>::infinity();
        constexpr T ninf = -std::numeric_limits<T>::infinity();

        static_assert(TRAP::Math::Equal(TRAP::Math::Round(max), max, Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Round(min), min, Epsilon));
        static_assert(TRAP::Math::IsNaN(TRAP::Math::Round(nan)));
        static_assert(TRAP::Math::IsInf(TRAP::Math::Round(inf)));
        static_assert(TRAP::Math::IsInf(TRAP::Math::Round(ninf)));
    }

    template<typename T>
    requires std::floating_point<T>
    void RunRoundEdgeRunTimeTests()
    {
        static constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        static constexpr T max = std::numeric_limits<T>::max();
        static constexpr T min = std::numeric_limits<T>::lowest();
        static constexpr T nan = std::numeric_limits<T>::quiet_NaN();
        static constexpr T inf = std::numeric_limits<T>::infinity();
        static constexpr T ninf = -std::numeric_limits<T>::infinity();

        REQUIRE(TRAP::Math::Equal(TRAP::Math::Round(max), max, Epsilon));
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Round(min), min, Epsilon));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Round(nan)));
        REQUIRE(TRAP::Math::IsInf(TRAP::Math::Round(inf)));
        REQUIRE(TRAP::Math::IsInf(TRAP::Math::Round(ninf)));
    }
}

TEST_CASE("TRAP::Math::Round()", "[math][generic][round]")
{
    SECTION("Scalar - f64")
    {
        RunRoundCompileTimeTests<f64>();
        RunRoundRunTimeTests<f64>();
        RunRoundEdgeCompileTimeTests<f64>();
        RunRoundEdgeRunTimeTests<f64>();
    }
    SECTION("Scalar - f32")
    {
        RunRoundCompileTimeTests<f32>();
        RunRoundRunTimeTests<f32>();
        RunRoundEdgeCompileTimeTests<f32>();
        RunRoundEdgeRunTimeTests<f32>();
    }

    SECTION("Vec2 - f64")
    {
        RunRoundRunTimeTests<TRAP::Math::Vec2d>();
        RunRoundVecCompileTimeTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - f32")
    {
        RunRoundRunTimeTests<TRAP::Math::Vec2f>();
        RunRoundVecCompileTimeTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - f64")
    {
        RunRoundRunTimeTests<TRAP::Math::Vec3d>();
        RunRoundVecCompileTimeTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - f32")
    {
        RunRoundRunTimeTests<TRAP::Math::Vec3f>();
        RunRoundVecCompileTimeTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - f64")
    {
        RunRoundRunTimeTests<TRAP::Math::Vec4d>();
        RunRoundVecCompileTimeTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - f32")
    {
        RunRoundRunTimeTests<TRAP::Math::Vec4f>();
        RunRoundVecCompileTimeTests<TRAP::Math::Vec4f>();
    }
}
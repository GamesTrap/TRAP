#include <cmath>
#include <limits>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires std::floating_point<T>
    consteval void RunTruncCompileTimeTests()
    {
        constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        static_assert(TRAP::Math::Equal(TRAP::Math::Trunc(T(   0.0f)), T( 0.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Trunc(T(-  0.0f)), T( 0.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Trunc(T(   0.1f)), T( 0.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Trunc(T(-  0.1f)), T( 0.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Trunc(T(   0.5f)), T( 0.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Trunc(T(-  0.5f)), T( 0.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Trunc(T(   0.9f)), T( 0.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Trunc(T(-  0.9f)), T( 0.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Trunc(T(   1.0f)), T( 1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Trunc(T(-  1.0f)), T(-1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Trunc(T(   1.5f)), T( 1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Trunc(T(-  1.5f)), T(-1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Trunc(T(   1.9f)), T( 1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Trunc(T(-  1.9f)), T(-1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Trunc(T(   4.2f)), T( 4.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Trunc(T(   4.5f)), T( 4.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Trunc(T(   4.7f)), T( 4.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Trunc(T(   5.0f)), T( 5.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Trunc(T(-  4.2f)), T(-4.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Trunc(T(-  4.5f)), T(-4.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Trunc(T(-  4.7f)), T(-4.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Trunc(T(-  5.0f)), T(-5.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Trunc(T( 42e32f)), T( 4.2e+33f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Trunc(T(-42e32f)), T(-4.2e+33f), Epsilon));
    }

    template<typename T>
    requires (TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>)
    consteval void RunTruncVecCompileTimeTests()
    {
        constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Trunc(T(   0.0f)), T( 0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Trunc(T(-  0.0f)), T( 0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Trunc(T(   0.1f)), T( 0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Trunc(T(-  0.1f)), T( 0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Trunc(T(   0.5f)), T( 0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Trunc(T(-  0.5f)), T( 0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Trunc(T(   0.9f)), T( 0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Trunc(T(-  0.9f)), T( 0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Trunc(T(   1.0f)), T( 1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Trunc(T(-  1.0f)), T(-1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Trunc(T(   1.5f)), T( 1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Trunc(T(-  1.5f)), T(-1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Trunc(T(   1.9f)), T( 1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Trunc(T(-  1.9f)), T(-1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Trunc(T(   4.2f)), T( 4.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Trunc(T(   4.5f)), T( 4.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Trunc(T(   4.7f)), T( 4.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Trunc(T(   5.0f)), T( 5.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Trunc(T(-  4.2f)), T(-4.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Trunc(T(-  4.5f)), T(-4.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Trunc(T(-  4.7f)), T(-4.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Trunc(T(-  5.0f)), T(-5.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Trunc(T( 42e32f)), T( 4.2e+33f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Trunc(T(-42e32f)), T(-4.2e+33f), Epsilon)));
    }

    template<typename T>
    requires std::floating_point<T> || (TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>)
    void RunTruncRunTimeTests()
    {
        static constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        static constexpr std::array<T, 24> values
        {
            T(0.0f), T(-0.0f), T(0.1f), T(-0.1f), T(0.5f), T(-0.5f), T(0.9f), T(-0.9f), T(1.0f), T(-1.0f),
            T(1.5f), T(-1.5f), T(1.9f), T(-1.9f), T(4.2f), T(4.5f), T(4.7f), T(5.0f), T(-4.2f), T(-4.5f),
            T(-4.7f), T(-5.0f), T(42e32f), T(-42e32f)
        };

        for(const T val : values)
        {
            if constexpr(std::floating_point<T>)
            {
                REQUIRE(TRAP::Math::Equal(TRAP::Math::Trunc(val), std::trunc(val), Epsilon));
            }
            else if constexpr(TRAP::Math::IsVec<T>)
            {
                REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Trunc(val), T(std::trunc(std::get<0>(val))), Epsilon)));
            }
        }
    }

    template<typename T>
    requires std::floating_point<T>
    consteval void RunTruncEdgeCompileTimeTests()
    {
        constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        constexpr T max = std::numeric_limits<T>::max();
        constexpr T min = std::numeric_limits<T>::lowest();
        constexpr T nan = std::numeric_limits<T>::quiet_NaN();
        constexpr T inf = std::numeric_limits<T>::infinity();
        constexpr T ninf = -std::numeric_limits<T>::infinity();

        static_assert(TRAP::Math::Equal(TRAP::Math::Trunc(max), max, Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Trunc(min), min, Epsilon));
        static_assert(TRAP::Math::IsNaN(TRAP::Math::Trunc(nan)));
        static_assert(TRAP::Math::IsInf(TRAP::Math::Trunc(inf)));
        static_assert(TRAP::Math::IsInf(TRAP::Math::Trunc(ninf)));
    }

    template<typename T>
    requires std::floating_point<T>
    void RunTruncEdgeRunTimeTests()
    {
        static constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        static constexpr T max = std::numeric_limits<T>::max();
        static constexpr T min = std::numeric_limits<T>::lowest();
        static constexpr T nan = std::numeric_limits<T>::quiet_NaN();
        static constexpr T inf = std::numeric_limits<T>::infinity();
        static constexpr T ninf = -std::numeric_limits<T>::infinity();

        REQUIRE(TRAP::Math::Equal(TRAP::Math::Trunc(max), max, Epsilon));
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Trunc(min), min, Epsilon));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Trunc(nan)));
        REQUIRE(TRAP::Math::IsInf(TRAP::Math::Trunc(inf)));
        REQUIRE(TRAP::Math::IsInf(TRAP::Math::Trunc(ninf)));
    }
}

TEST_CASE("TRAP::Math::Trunc()", "[math][generic][trunc]")
{
    SECTION("Scalar - f64")
    {
        RunTruncRunTimeTests<f64>();
        RunTruncCompileTimeTests<f64>();
        RunTruncEdgeCompileTimeTests<f64>();
        RunTruncEdgeRunTimeTests<f64>();
    }
    SECTION("Scalar - f32")
    {
        RunTruncRunTimeTests<f32>();
        RunTruncCompileTimeTests<f32>();
        RunTruncEdgeCompileTimeTests<f32>();
        RunTruncEdgeRunTimeTests<f32>();
    }

    SECTION("Vec2 - f64")
    {
        RunTruncRunTimeTests<TRAP::Math::Vec2d>();
        RunTruncVecCompileTimeTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - f32")
    {
        RunTruncRunTimeTests<TRAP::Math::Vec2f>();
        RunTruncVecCompileTimeTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - f64")
    {
        RunTruncRunTimeTests<TRAP::Math::Vec3d>();
        RunTruncVecCompileTimeTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - f32")
    {
        RunTruncRunTimeTests<TRAP::Math::Vec3f>();
        RunTruncVecCompileTimeTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - f64")
    {
        RunTruncRunTimeTests<TRAP::Math::Vec4d>();
        RunTruncVecCompileTimeTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - f32")
    {
        RunTruncRunTimeTests<TRAP::Math::Vec4f>();
        RunTruncVecCompileTimeTests<TRAP::Math::Vec4f>();
    }
}
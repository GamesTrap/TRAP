#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires std::floating_point<T>
    consteval void RunCeilCompileTimeTests()
    {
        constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        static_assert(TRAP::Math::Equal(TRAP::Math::Ceil(T(   0.0f)), T(    0.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Ceil(T(-  0.0f)), T(    0.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Ceil(T(   0.1f)), T(    1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Ceil(T(-  0.1f)), T(    0.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Ceil(T(   0.5f)), T(    1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Ceil(T(-  0.5f)), T(    0.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Ceil(T(   0.9f)), T(    1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Ceil(T(-  0.9f)), T(    0.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Ceil(T(   1.0f)), T(    1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Ceil(T(-  1.0f)), T(-   1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Ceil(T(   1.5f)), T(    2.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Ceil(T(-  1.5f)), T(-   1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Ceil(T(   1.9f)), T(    2.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Ceil(T(-  1.9f)), T(-   1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Ceil(T(   4.2f)), T(    5.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Ceil(T(   4.5f)), T(    5.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Ceil(T(   4.7f)), T(    5.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Ceil(T(   5.0f)), T(    5.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Ceil(T(-  4.2f)), T(-   4.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Ceil(T(-  4.5f)), T(-   4.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Ceil(T(-  4.7f)), T(-   4.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Ceil(T(-  5.0f)), T(-   5.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Ceil(T( 42e32f)), T( 4.2e+33f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Ceil(T(-42e32f)), T(-4.2e+33f), Epsilon));
    }

    template<typename T>
    requires (TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>)
    consteval void RunCeilVecCompileTimeTests()
    {
        constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Ceil(T(   0.0f)), T(    0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Ceil(T(-  0.0f)), T(    0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Ceil(T(   0.1f)), T(    1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Ceil(T(-  0.1f)), T(    0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Ceil(T(   0.5f)), T(    1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Ceil(T(-  0.5f)), T(    0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Ceil(T(   0.9f)), T(    1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Ceil(T(-  0.9f)), T(    0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Ceil(T(   1.0f)), T(    1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Ceil(T(-  1.0f)), T(-   1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Ceil(T(   1.5f)), T(    2.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Ceil(T(-  1.5f)), T(-   1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Ceil(T(   1.9f)), T(    2.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Ceil(T(-  1.9f)), T(-   1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Ceil(T(   4.2f)), T(    5.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Ceil(T(   4.5f)), T(    5.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Ceil(T(   4.7f)), T(    5.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Ceil(T(   5.0f)), T(    5.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Ceil(T(-  4.2f)), T(-   4.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Ceil(T(-  4.5f)), T(-   4.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Ceil(T(-  4.7f)), T(-   4.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Ceil(T(-  5.0f)), T(-   5.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Ceil(T( 42e32f)), T( 4.2e+33f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Ceil(T(-42e32f)), T(-4.2e+33f), Epsilon)));
    }

    template<typename T>
    requires std::floating_point<T> || (TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>)
    void RunCeilRunTimeTests()
    {
        static constexpr std::array<T, 24> values
        {
            static_cast<T>(0.0f), static_cast<T>(-0.0f), static_cast<T>(0.1f), static_cast<T>(-0.1f),
            static_cast<T>(0.5f), static_cast<T>(-0.5f), static_cast<T>(0.9f), static_cast<T>(-0.9f),
            static_cast<T>(1.0f), static_cast<T>(-1.0f), static_cast<T>(1.5f), static_cast<T>(-1.5f),
            static_cast<T>(1.9f), static_cast<T>(-1.9f), static_cast<T>(4.2f), static_cast<T>(4.5f),
            static_cast<T>(4.7f), static_cast<T>(5.0f), static_cast<T>(-4.2f), static_cast<T>(-4.5f),
            static_cast<T>(-4.7f), static_cast<T>(-5.0f), static_cast<T>(42e32f), static_cast<T>(-42e32f)
        };

        static constexpr T Epsilon = std::numeric_limits<T>::epsilon();
        for(const T val : values)
        {
            if constexpr(std::floating_point<T>)
            {
                REQUIRE(TRAP::Math::Equal(TRAP::Math::Ceil(val), T(std::ceil(val)), Epsilon));
            }
            else if constexpr(TRAP::Math::IsVec<T>)
            {
                REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Ceil(val), T(std::ceil(std::get<0>(val))), Epsilon)));
            }
        }
    }

    template<typename T>
    requires std::floating_point<T>
    consteval void RunCeilEdgeCompileTimeTests()
    {
        constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        constexpr T max = std::numeric_limits<T>::max();
        static_assert(TRAP::Math::Equal(TRAP::Math::Ceil(max), max, Epsilon));
        constexpr T min = std::numeric_limits<T>::lowest();
        static_assert(TRAP::Math::Equal(TRAP::Math::Ceil(min), min, Epsilon));
        constexpr T nan = std::numeric_limits<T>::quiet_NaN();
        static_assert(TRAP::Math::IsNaN(TRAP::Math::Ceil(nan)));
        constexpr T inf = std::numeric_limits<T>::infinity();
        static_assert(TRAP::Math::Equal(TRAP::Math::Ceil(inf), inf));
        constexpr T ninf = -std::numeric_limits<T>::infinity();
        static_assert(TRAP::Math::Equal(TRAP::Math::Ceil(ninf), ninf));
    }

    template<typename T>
    requires std::floating_point<T>
    void RunCeilEdgeRunTimeTests()
    {
        static constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        static constexpr T max = std::numeric_limits<T>::max();
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Ceil(max), max, Epsilon));
        static constexpr T min = std::numeric_limits<T>::lowest();
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Ceil(min), min, Epsilon));
        static constexpr T nan = std::numeric_limits<T>::quiet_NaN();
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Ceil(nan)));
        static constexpr T inf = std::numeric_limits<T>::infinity();
        REQUIRE(TRAP::Math::IsInf(TRAP::Math::Ceil(inf)));
        static constexpr T ninf = -std::numeric_limits<T>::infinity();
        REQUIRE(TRAP::Math::IsInf(TRAP::Math::Ceil(ninf)));
    }
}

TEST_CASE("TRAP::Math::Ceil()", "[math][generic][ceil]")
{
    SECTION("Scalar - double")
    {
        RunCeilRunTimeTests<double>();
        RunCeilCompileTimeTests<double>();
        RunCeilEdgeRunTimeTests<double>();
        RunCeilEdgeCompileTimeTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunCeilRunTimeTests<float>();
        RunCeilCompileTimeTests<float>();
        RunCeilEdgeRunTimeTests<float>();
        RunCeilEdgeCompileTimeTests<float>();
    }

    SECTION("Vec2 - double")
    {
        RunCeilRunTimeTests<TRAP::Math::Vec2d>();
        RunCeilVecCompileTimeTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunCeilRunTimeTests<TRAP::Math::Vec2f>();
        RunCeilVecCompileTimeTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - double")
    {
        RunCeilRunTimeTests<TRAP::Math::Vec3d>();
        RunCeilVecCompileTimeTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunCeilRunTimeTests<TRAP::Math::Vec3f>();
        RunCeilVecCompileTimeTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - double")
    {
        RunCeilRunTimeTests<TRAP::Math::Vec4d>();
        RunCeilVecCompileTimeTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunCeilRunTimeTests<TRAP::Math::Vec4f>();
        RunCeilVecCompileTimeTests<TRAP::Math::Vec4f>();
    }
}
#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>
#include <array>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::floating_point<T>
consteval void RunCompileTimeRadiansScalarTests()
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    static_assert(TRAP::Math::Equal(TRAP::Math::Radians(T(0.0f)), T(0.0f), Epsilon));
    static_assert(TRAP::Math::Equal(TRAP::Math::Radians(T(45.0f)), T(0.785398f), T(0.000001f)));
    static_assert(TRAP::Math::Equal(TRAP::Math::Radians(T(90.0f)), T(1.5708f), T(0.00001f)));
    static_assert(TRAP::Math::Equal(TRAP::Math::Radians(T(180.0f)), T(3.14159f), T(0.00001f)));
    static_assert(TRAP::Math::Equal(TRAP::Math::Radians(T(270.0f)), T(4.71239f), T(0.000001f)));
    static_assert(TRAP::Math::Equal(TRAP::Math::Radians(T(-45.0f)), T(-0.785398f), T(0.000001f)));
    static_assert(TRAP::Math::Equal(TRAP::Math::Radians(T(-90.0f)), T(-1.5708f), T(0.00001f)));
    static_assert(TRAP::Math::Equal(TRAP::Math::Radians(T(-180.0f)), T(-3.14159f), T(0.00001f)));
    static_assert(TRAP::Math::Equal(TRAP::Math::Radians(T(-270.0f)), T(-4.71239f), T(0.000001f)));
    static_assert(TRAP::Math::Equal(TRAP::Math::Radians(T(360.0f)), T(6.28319f), T(0.00001f)));
}

template<typename T>
requires std::floating_point<T>
consteval void RunCompileTimeRadiansScalarEdgeTests()
{
    constexpr T nan = std::numeric_limits<T>::quiet_NaN();
    constexpr T inf = std::numeric_limits<T>::infinity();
    constexpr T ninf = -std::numeric_limits<T>::infinity();

    static_assert(TRAP::Math::IsInf(inf));
    static_assert(TRAP::Math::IsInf(ninf));
    static_assert(TRAP::Math::IsNaN(nan));
}

template<typename T>
requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
consteval void RunCompileTimeRadiansVecTests()
{
    {
        constexpr T degrees(TRAP::Math::tVec4<typename T::value_type>(0.0f, 45.0f, 90.0f, 180.0f));
        constexpr T expected(TRAP::Math::tVec4<typename T::value_type>(0.0f, 0.785398f, 1.5708f, 3.14159f));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Radians(degrees), expected, typename T::value_type(0.00001f))));
    }
    {
        constexpr T degrees(TRAP::Math::tVec4<typename T::value_type>(270.0f, -45.0f, -90.0f, -180.0f));
        constexpr T expected(TRAP::Math::tVec4<typename T::value_type>(4.71239f, -0.785398f, -1.5708f, -3.14159f));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Radians(degrees), expected, typename T::value_type(0.00001f))));
    }
    {
        constexpr T degrees(TRAP::Math::tVec4<typename T::value_type>(-270.0f, 360.0f, 85.0f, -99.0f));
        constexpr T expected(TRAP::Math::tVec4<typename T::value_type>(-4.71239f, 6.28319f, 1.48353f, -1.72788f));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Radians(degrees), expected, typename T::value_type(0.00001f))));
    }
}

template<typename T>
requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
void RunCompileTimeRadiansVecEdgeTests()
{
    static constexpr typename T::value_type nan = std::numeric_limits<typename T::value_type>::quiet_NaN();
    static constexpr typename T::value_type inf = std::numeric_limits<typename T::value_type>::infinity();
    static constexpr typename T::value_type ninf = -std::numeric_limits<typename T::value_type>::infinity();

    {
        static_assert(TRAP::Math::All(TRAP::Math::IsInf(TRAP::Math::Radians(T(inf)))));
        static_assert(TRAP::Math::All(TRAP::Math::IsInf(TRAP::Math::Radians(T(ninf)))));
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::Radians(T(nan)))));
    }
}

TEST_CASE("TRAP::Math::Radians()", "[math][generic][radians]")
{
    SECTION("Scalar - double")
    {
        RunCompileTimeRadiansScalarTests<double>();
        RunCompileTimeRadiansScalarEdgeTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunCompileTimeRadiansScalarTests<float>();
        RunCompileTimeRadiansScalarEdgeTests<float>();
    }

    SECTION("Vec2 - double")
    {
        RunCompileTimeRadiansVecTests<TRAP::Math::Vec2d>();
        RunCompileTimeRadiansVecEdgeTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunCompileTimeRadiansVecTests<TRAP::Math::Vec2f>();
        RunCompileTimeRadiansVecEdgeTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - double")
    {
        RunCompileTimeRadiansVecTests<TRAP::Math::Vec3d>();
        RunCompileTimeRadiansVecEdgeTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunCompileTimeRadiansVecTests<TRAP::Math::Vec3f>();
        RunCompileTimeRadiansVecEdgeTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - double")
    {
        RunCompileTimeRadiansVecTests<TRAP::Math::Vec4d>();
        RunCompileTimeRadiansVecEdgeTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunCompileTimeRadiansVecTests<TRAP::Math::Vec4f>();
        RunCompileTimeRadiansVecEdgeTests<TRAP::Math::Vec4f>();
    }
}
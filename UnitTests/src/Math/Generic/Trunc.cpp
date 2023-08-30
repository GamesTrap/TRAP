#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::floating_point<T> || (TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>)
void RunTruncTests()
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    constexpr std::array<T, 24> values
    {
        T(0.0f), T(-0.0f), T(0.1f), T(-0.1f), T(0.5f), T(-0.5f), T(0.9f), T(-0.9f), T(1.0f), T(-1.0f),
        T(1.5f), T(-1.5f), T(1.9f), T(-1.9f), T(4.2f), T(4.5f), T(4.7f), T(5.0f), T(-4.2f), T(-4.5f),
        T(-4.7f), T(-5.0f), T(42e32f), T(-42e32f)
    };

    for(const T val : values)
    {
        if constexpr(std::floating_point<T>)
        {
            REQUIRE_THAT(TRAP::Math::Trunc(val), Catch::Matchers::WithinRel(std::trunc(val), Epsilon));
        }
        else if constexpr(TRAP::Math::IsVec<T>)
        {
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Trunc(val), T(std::trunc(std::get<0>(val))), Epsilon)));
        }
    }
}

template<typename T>
requires std::floating_point<T>
void RunTruncEdgeTests()
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    constexpr T max = std::numeric_limits<T>::max();
    constexpr T min = std::numeric_limits<T>::lowest();
    constexpr T nan = std::numeric_limits<T>::quiet_NaN();
    constexpr T inf = std::numeric_limits<T>::infinity();
    constexpr T ninf = -std::numeric_limits<T>::infinity();

    REQUIRE(TRAP::Math::Equal(TRAP::Math::Trunc(max), max, Epsilon));
    REQUIRE(TRAP::Math::Equal(TRAP::Math::Trunc(min), min, Epsilon));
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Trunc(nan)));
    REQUIRE(TRAP::Math::IsInf(TRAP::Math::Trunc(inf)));
    REQUIRE(TRAP::Math::IsInf(TRAP::Math::Trunc(ninf)));
}

TEST_CASE("TRAP::Math::Trunc()", "[math][generic][trunc]")
{
    SECTION("Scalar - double")
    {
        RunTruncTests<double>();
        RunTruncEdgeTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunTruncTests<float>();
        RunTruncEdgeTests<float>();
    }

    SECTION("Vec2 - double")
    {
        RunTruncTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunTruncTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - double")
    {
        RunTruncTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunTruncTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - double")
    {
        RunTruncTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunTruncTests<TRAP::Math::Vec4f>();
    }
}
#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::floating_point<T> || (TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>)
void RunRoundTests()
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
            REQUIRE_THAT(TRAP::Math::Round(val), Catch::Matchers::WithinRel(std::round(val), Epsilon));
        }
        else if constexpr(TRAP::Math::IsVec<T>)
        {
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Round(val), T(std::round(std::get<0>(val))), Epsilon)));
        }
    }
}

template<typename T>
requires std::floating_point<T>
void RunRoundEdgeTests()
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    constexpr T max = std::numeric_limits<T>::max();
    constexpr T min = std::numeric_limits<T>::lowest();
    constexpr T nan = std::numeric_limits<T>::quiet_NaN();
    constexpr T inf = std::numeric_limits<T>::infinity();
    constexpr T ninf = -std::numeric_limits<T>::infinity();

    REQUIRE(TRAP::Math::Equal(TRAP::Math::Round(max), max, Epsilon));
    REQUIRE(TRAP::Math::Equal(TRAP::Math::Round(min), min, Epsilon));
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Round(nan)));
    REQUIRE(TRAP::Math::IsInf(TRAP::Math::Round(inf)));
    REQUIRE(TRAP::Math::IsInf(TRAP::Math::Round(ninf)));
}

TEST_CASE("TRAP::Math::Round()", "[math][generic][round]")
{
    SECTION("Scalar - double")
    {
        RunRoundTests<double>();
        RunRoundEdgeTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunRoundTests<float>();
        RunRoundEdgeTests<float>();
    }

    SECTION("Vec2 - double")
    {
        RunRoundTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunRoundTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - double")
    {
        RunRoundTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunRoundTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - double")
    {
        RunRoundTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunRoundTests<TRAP::Math::Vec4f>();
    }
}
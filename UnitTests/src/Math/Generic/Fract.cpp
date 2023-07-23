#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::floating_point<T> || (TRAP::Math::IsVec<T> && std::floating_point<typename T::valueType>)
void RunFractTests()
{
    if constexpr(std::floating_point<T>)
    {
        constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        constexpr std::array<std::pair<T, T>, 1> values
        {
            std::pair{T(1.5), T(0.5)}
        };

        for(const auto& [val, expected] : values)
        {
            REQUIRE_THAT(TRAP::Math::Fract(val), Catch::Matchers::WithinRel(expected, Epsilon));
        }
    }
    else if constexpr(TRAP::Math::IsVec<T> && std::floating_point<typename T::valueType>)
    {
        constexpr typename T::valueType Epsilon = std::numeric_limits<typename T::valueType>::epsilon();

        constexpr std::array<std::pair<T, T>, 1> values
        {
            std::pair<T, T>{TRAP::Math::Vec<4, typename T::valueType>(1.1, 1.2, 1.5, 1.7),
                            TRAP::Math::Vec<4, typename T::valueType>(0.1, 0.2, 0.5, 0.7)},
        };

        for(const auto& [val, expected] : values)
        {
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Fract(val), expected, Epsilon)));
        }
    }
}

template<typename T>
requires std::floating_point<T>
void RunFractEdgeTests()
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    constexpr T max = std::numeric_limits<T>::max();
    REQUIRE(TRAP::Math::Equal(TRAP::Math::Fract(max), T(0.0), Epsilon));
    constexpr T min = std::numeric_limits<T>::lowest();
    REQUIRE(TRAP::Math::Equal(TRAP::Math::Fract(min), T(0.0), Epsilon));
    constexpr T nan = std::numeric_limits<T>::quiet_NaN();
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Fract(nan)));
    constexpr T inf = std::numeric_limits<T>::infinity();
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Fract(inf)));
    constexpr T ninf = -std::numeric_limits<T>::infinity();
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Fract(ninf)));
}

TEST_CASE("TRAP::Math::Fract()", "[math][generic][fract]")
{
    SECTION("Scalar - double")
    {
        RunFractTests<double>();
        RunFractEdgeTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunFractTests<float>();
        RunFractEdgeTests<float>();
    }

    SECTION("Vec2 - double")
    {
        RunFractTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunFractTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - double")
    {
        RunFractTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunFractTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - double")
    {
        RunFractTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunFractTests<TRAP::Math::Vec4f>();
    }
}
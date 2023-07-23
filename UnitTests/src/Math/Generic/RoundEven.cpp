#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::floating_point<T> || (TRAP::Math::IsVec<T> && std::floating_point<typename T::valueType>)
void RunRoundEvenTests()
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    constexpr std::array<std::pair<T, T>, 48> values
    {
        std::pair<T, T>{-1.5, -2.0}, std::pair<T, T>{ 1.5,  2.0}, std::pair<T, T>{-2.5, -2.0},
        std::pair<T, T>{ 2.5,  2.0}, std::pair<T, T>{-3.5, -4.0}, std::pair<T, T>{ 3.5,  4.0},
        std::pair<T, T>{-4.5, -4.0}, std::pair<T, T>{ 4.5,  4.0}, std::pair<T, T>{-5.5, -6.0},
        std::pair<T, T>{ 5.5,  6.0}, std::pair<T, T>{-6.5, -6.0}, std::pair<T, T>{ 6.5,  6.0},
        std::pair<T, T>{-7.5, -8.0}, std::pair<T, T>{ 7.5,  8.0},

        std::pair<T, T>{-2.4, -2.0}, std::pair<T, T>{ 2.4,  2.0}, std::pair<T, T>{-2.6, -3.0},
        std::pair<T, T>{ 2.6,  3.0}, std::pair<T, T>{-2.0, -2.0}, std::pair<T, T>{ 2.0,  2.0},

        std::pair<T, T>{ 0.0,  0.0}, std::pair<T, T>{ 0.5,  0.0}, std::pair<T, T>{ 1.0,  1.0},
        std::pair<T, T>{ 0.1,  0.0}, std::pair<T, T>{ 0.9,  1.0}, std::pair<T, T>{ 1.5,  2.0},
        std::pair<T, T>{ 1.9,  2.0},

        std::pair<T, T>{-0.0,  0.0}, std::pair<T, T>{-0.5, -0.0}, std::pair<T, T>{-1.0, -1.0},
        std::pair<T, T>{-0.1,  0.0}, std::pair<T, T>{-0.9, -1.0}, std::pair<T, T>{-1.5, -2.0},
        std::pair<T, T>{-1.9, -2.0},

        std::pair<T, T>{ 1.5,  2.0}, std::pair<T, T>{ 2.5,  2.0}, std::pair<T, T>{ 3.5,  4.0},
        std::pair<T, T>{ 4.5,  4.0}, std::pair<T, T>{ 5.5,  6.0}, std::pair<T, T>{ 6.5,  6.0},
        std::pair<T, T>{ 7.5,  8.0},

        std::pair<T, T>{-1.5, -2.0}, std::pair<T, T>{-2.5, -2.0}, std::pair<T, T>{-3.5, -4.0},
        std::pair<T, T>{-4.5, -4.0}, std::pair<T, T>{-5.5, -6.0}, std::pair<T, T>{-6.5, -6.0},
        std::pair<T, T>{-7.5, -8.0},
    };

    for(const auto& [val, expected] : values)
    {
        if constexpr(std::floating_point<T>)
        {
            REQUIRE_THAT(TRAP::Math::RoundEven(val), Catch::Matchers::WithinRel(expected, Epsilon));
        }
        else if constexpr(TRAP::Math::IsVec<T>)
        {
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(val), T(expected), Epsilon)));
        }
    }
}

template<typename T>
requires std::floating_point<T>
void RunRoundEvenEdgeTests()
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    constexpr T max = std::numeric_limits<T>::max();
    constexpr T min = std::numeric_limits<T>::lowest();

    REQUIRE(TRAP::Math::Equal(TRAP::Math::RoundEven(max), max, Epsilon));
    REQUIRE(TRAP::Math::Equal(TRAP::Math::RoundEven(min), min, Epsilon));
}

TEST_CASE("TRAP::Math::RoundEven()", "[math][generic][roundeven]")
{
    SECTION("Scalar - double")
    {
        RunRoundEvenTests<double>();
        RunRoundEvenEdgeTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunRoundEvenTests<float>();
        RunRoundEvenEdgeTests<float>();
    }

    SECTION("Vec2 - double")
    {
        RunRoundEvenTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunRoundEvenTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - double")
    {
        RunRoundEvenTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunRoundEvenTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - double")
    {
        RunRoundEvenTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunRoundEvenTests<TRAP::Math::Vec4f>();
    }
}
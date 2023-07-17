#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::floating_point<T> || (TRAP::Math::IsVec<T> && std::floating_point<typename T::valueType>)
void RunRoundEvenTests(const T val, const T expected)
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();
    if constexpr(std::floating_point<T>)
    {
        REQUIRE_THAT(TRAP::Math::RoundEven(val), Catch::Matchers::WithinRel(expected, Epsilon));
    }
    else if constexpr(TRAP::Math::IsVec<T>)
    {
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(val), T(expected), Epsilon)));
    }
}

TEST_CASE("TRAP::Math::RoundEven()", "[math][generic][roundeven]")
{
    const auto val = GENERATE(values(
        {
            std::pair{-1.5, -2.0},
            std::pair{ 1.5,  2.0},
            std::pair{-2.5, -2.0},
            std::pair{ 2.5,  2.0},
            std::pair{-3.5, -4.0},
            std::pair{ 3.5,  4.0},
            std::pair{-4.5, -4.0},
            std::pair{ 4.5,  4.0},
            std::pair{-5.5, -6.0},
            std::pair{ 5.5,  6.0},
            std::pair{-6.5, -6.0},
            std::pair{ 6.5,  6.0},
            std::pair{-7.5, -8.0},
            std::pair{ 7.5,  8.0},

            std::pair{-2.4, -2.0},
            std::pair{ 2.4,  2.0},
            std::pair{-2.6, -3.0},
            std::pair{ 2.6,  3.0},
            std::pair{-2.0, -2.0},
            std::pair{ 2.0,  2.0},

            std::pair{ 0.0,  0.0},
            std::pair{ 0.5,  0.0},
            std::pair{ 1.0,  1.0},
            std::pair{ 0.1,  0.0},
            std::pair{ 0.9,  1.0},
            std::pair{ 1.5,  2.0},
            std::pair{ 1.9,  2.0},

            std::pair{-0.0,  0.0},
            std::pair{-0.5, -0.0},
            std::pair{-1.0, -1.0},
            std::pair{-0.1,  0.0},
            std::pair{-0.9, -1.0},
            std::pair{-1.5, -2.0},
            std::pair{-1.9, -2.0},

            std::pair{ 1.5,  2.0},
            std::pair{ 2.5,  2.0},
            std::pair{ 3.5,  4.0},
            std::pair{ 4.5,  4.0},
            std::pair{ 5.5,  6.0},
            std::pair{ 6.5,  6.0},
            std::pair{ 7.5,  8.0},

            std::pair{-1.5, -2.0},
            std::pair{-2.5, -2.0},
            std::pair{-3.5, -4.0},
            std::pair{-4.5, -4.0},
            std::pair{-5.5, -6.0},
            std::pair{-6.5, -6.0},
            std::pair{-7.5, -8.0},
        }));

    SECTION("Scalar - double")
    {
        RunRoundEvenTests(val.first, val.second);
    }
    SECTION("Scalar - float")
    {
        RunRoundEvenTests(static_cast<float>(val.first), static_cast<float>(val.second));
    }

    SECTION("Vec2 - double")
    {
        RunRoundEvenTests(TRAP::Math::Vec2d(static_cast<double>(val.first)), TRAP::Math::Vec2d(static_cast<double>(val.second)));
    }
    SECTION("Vec2 - float")
    {
        RunRoundEvenTests(TRAP::Math::Vec2f(static_cast<float>(val.first)), TRAP::Math::Vec2f(static_cast<float>(val.second)));
    }

    SECTION("Vec3 - double")
    {
        RunRoundEvenTests(TRAP::Math::Vec3d(static_cast<double>(val.first)), TRAP::Math::Vec3d(static_cast<double>(val.second)));
    }
    SECTION("Vec3 - float")
    {
        RunRoundEvenTests(TRAP::Math::Vec3f(static_cast<float>(val.first)), TRAP::Math::Vec3f(static_cast<float>(val.second)));
    }

    SECTION("Vec4 - double")
    {
        RunRoundEvenTests(TRAP::Math::Vec4d(static_cast<double>(val.first)), TRAP::Math::Vec4d(static_cast<double>(val.second)));
    }
    SECTION("Vec4 - float")
    {
        RunRoundEvenTests(TRAP::Math::Vec4f(static_cast<float>(val.first)), TRAP::Math::Vec4f(static_cast<float>(val.second)));
    }
}
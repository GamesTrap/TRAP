#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::floating_point<T> || (TRAP::Math::IsVec<T> && std::floating_point<typename T::valueType>)
void RunFractTests(const T val, const T expected)
{
    if constexpr(std::floating_point<T>)
    {
        constexpr T Epsilon = std::numeric_limits<T>::epsilon();
        REQUIRE_THAT(TRAP::Math::Fract(val), Catch::Matchers::WithinRel(expected, Epsilon));
    }
    else if constexpr(TRAP::Math::IsVec<T> && std::floating_point<typename T::valueType>)
    {
        constexpr typename T::valueType Epsilon = std::numeric_limits<typename T::valueType>::epsilon();
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Fract(val), expected, Epsilon)));
    }
}

TEST_CASE("TRAP::Math::Fract()", "[math][generic][fract]")
{
    {
        const auto val = GENERATE(values(
            {
                std::pair{1.5, 0.5},
            }));

        SECTION("Scalar - double")
        {
            RunFractTests(val.first, val.second);
        }
        SECTION("Scalar - float")
        {
            RunFractTests(static_cast<float>(val.first), static_cast<float>(val.second));
        }
    }

    const auto val = GENERATE(values(
        {
            std::pair{TRAP::Math::Vec4d(1.1, 1.2, 1.5, 1.7), TRAP::Math::Vec4d(0.1, 0.2, 0.5, 0.7)},
        }));

    SECTION("Vec2 - double")
    {
        RunFractTests(TRAP::Math::Vec2d(val.first), TRAP::Math::Vec2d(val.second));
    }
    SECTION("Vec2 - float")
    {
        RunFractTests(TRAP::Math::Vec2f(val.first), TRAP::Math::Vec2f(val.second));
    }

    SECTION("Vec3 - double")
    {
        RunFractTests(TRAP::Math::Vec3d(val.first), TRAP::Math::Vec3d(val.second));
    }
    SECTION("Vec3 - float")
    {
        RunFractTests(TRAP::Math::Vec3f(val.first), TRAP::Math::Vec3f(val.second));
    }

    SECTION("Vec4 - double")
    {
        RunFractTests(TRAP::Math::Vec4d(val.first), TRAP::Math::Vec4d(val.second));
    }
    SECTION("Vec4 - float")
    {
        RunFractTests(TRAP::Math::Vec4f(val.first), TRAP::Math::Vec4f(val.second));
    }
}
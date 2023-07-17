#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::is_arithmetic_v<T> || (TRAP::Math::IsVec<T> && std::floating_point<typename T::valueType>)
void RunModfTests(const T x, const T expected, const T expectedI)
{
    if constexpr(std::is_arithmetic_v<T>)
    {
        if constexpr(std::floating_point<T>)
        {
            constexpr T Epsilon = std::numeric_limits<T>::epsilon();
            T i{};
            REQUIRE_THAT(TRAP::Math::Modf(x, i), Catch::Matchers::WithinRel(expected, Epsilon));
            REQUIRE_THAT(i, Catch::Matchers::WithinRel(expectedI, Epsilon));
        }
        else if constexpr(std::integral<T>)
        {
            T i{};
            REQUIRE(TRAP::Math::Modf(x, i) == expected);
            REQUIRE(i == expectedI);
        }
    }
    else if constexpr(TRAP::Math::IsVec<T> && std::floating_point<typename T::valueType>)
    {
        constexpr typename T::valueType Epsilon = std::numeric_limits<typename T::valueType>::epsilon();
        T i{};
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Modf(x, i), expected, Epsilon)));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(i, expectedI, Epsilon)));
    }
}

TEST_CASE("TRAP::Math::Modf()", "[math][generic][modf]")
{
    {
        const auto val = GENERATE(values(
            {
                std::tuple{1.5, 0.5, 1.0}
            }));

        SECTION("Scalar - double")
        {
            RunModfTests(std::get<0>(val), std::get<1>(val), std::get<2>(val));
        }
        SECTION("Scalar - float")
        {
            RunModfTests(static_cast<float>(std::get<0>(val)), static_cast<float>(std::get<1>(val)), static_cast<float>(std::get<2>(val)));
        }
    }

    const auto val = GENERATE(values(
        {
            std::tuple{TRAP::Math::Vec4d{1.1, 1.2, 1.5, 1.7}, TRAP::Math::Vec4d{0.1, 0.2, 0.5, 0.7}, TRAP::Math::Vec4d(1.0)}
        }));

    SECTION("Vec2 - double")
    {
        RunModfTests(TRAP::Math::Vec2d(std::get<0>(val)), TRAP::Math::Vec2d(std::get<1>(val)), TRAP::Math::Vec2d(std::get<2>(val)));
    }
    SECTION("Vec2 - float")
    {
        RunModfTests(TRAP::Math::Vec2f(std::get<0>(val)), TRAP::Math::Vec2f(std::get<1>(val)), TRAP::Math::Vec2f(std::get<2>(val)));
    }

    SECTION("Vec3 - double")
    {
        RunModfTests(TRAP::Math::Vec3d(std::get<0>(val)), TRAP::Math::Vec3d(std::get<1>(val)), TRAP::Math::Vec3d(std::get<2>(val)));
    }
    SECTION("Vec3 - float")
    {
        RunModfTests(TRAP::Math::Vec3f(std::get<0>(val)), TRAP::Math::Vec3f(std::get<1>(val)), TRAP::Math::Vec3f(std::get<2>(val)));
    }

    SECTION("Vec4 - double")
    {
        RunModfTests(TRAP::Math::Vec4d(std::get<0>(val)), TRAP::Math::Vec4d(std::get<1>(val)), TRAP::Math::Vec4d(std::get<2>(val)));
    }
    SECTION("Vec4 - float")
    {
        RunModfTests(TRAP::Math::Vec4f(std::get<0>(val)), TRAP::Math::Vec4f(std::get<1>(val)), TRAP::Math::Vec4f(std::get<2>(val)));
    }
}
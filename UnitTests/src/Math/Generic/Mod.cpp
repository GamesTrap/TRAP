#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T, typename X>
requires (std::is_arithmetic_v<T> || (TRAP::Math::IsVec<T> && std::floating_point<typename T::valueType>)) &&
         (std::is_arithmetic_v<X> || (TRAP::Math::IsVec<X> && std::floating_point<typename X::valueType>))
void RunModTests(const T x, const X y, const T expected)
{
    if constexpr(std::is_arithmetic_v<T>)
    {
        if constexpr(std::floating_point<T>)
        {
            constexpr T Epsilon = std::numeric_limits<T>::epsilon();
            REQUIRE_THAT(TRAP::Math::Mod(x, y), Catch::Matchers::WithinRel(expected, Epsilon));
        }
        else if constexpr(std::integral<T>)
        {
            REQUIRE(TRAP::Math::Mod(x, y) == expected);
        }
    }
    else if constexpr(TRAP::Math::IsVec<T> && std::floating_point<typename T::valueType>)
    {
        constexpr typename T::valueType Epsilon = std::numeric_limits<typename T::valueType>::epsilon();
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Mod(x, y), expected, Epsilon)));
    }
}

TEST_CASE("TRAP::Math::Mod()", "[math][generic][mod]")
{
    const auto val = GENERATE(values(
        {
            std::tuple{ 1.5,  1.0,  0.5},
            std::tuple{-0.2,  1.0,  0.8},
            std::tuple{ 3.0,  2.0,  1.0},
        }));

    SECTION("Scalar - int8_t")
    {
        RunModTests(static_cast<int8_t>(std::get<0>(val)), static_cast<int8_t>(std::get<1>(val)), static_cast<int8_t>(std::get<2>(val)));
    }
    SECTION("Scalar - uint8_t")
    {
        RunModTests(static_cast<uint8_t>(std::get<0>(val)), static_cast<uint8_t>(std::get<1>(val)), static_cast<uint8_t>(std::get<2>(val)));
    }
    SECTION("Scalar - int16_t")
    {
        RunModTests(static_cast<int16_t>(std::get<0>(val)), static_cast<int16_t>(std::get<1>(val)), static_cast<int16_t>(std::get<2>(val)));
    }
    SECTION("Scalar - uint16_t")
    {
        RunModTests(static_cast<uint16_t>(std::get<0>(val)), static_cast<uint16_t>(std::get<1>(val)), static_cast<uint16_t>(std::get<2>(val)));
    }
    SECTION("Scalar - int32_t")
    {
        RunModTests(static_cast<int32_t>(std::get<0>(val)), static_cast<int32_t>(std::get<1>(val)), static_cast<int32_t>(std::get<2>(val)));
    }
    SECTION("Scalar - uint32_t")
    {
        RunModTests(static_cast<uint32_t>(std::get<0>(val)), static_cast<uint32_t>(std::get<1>(val)), static_cast<uint32_t>(std::get<2>(val)));
    }
    SECTION("Scalar - int64_t")
    {
        RunModTests(static_cast<int64_t>(std::get<0>(val)), static_cast<int64_t>(std::get<1>(val)), static_cast<int64_t>(std::get<2>(val)));
    }
    SECTION("Scalar - uint64_t")
    {
        RunModTests(static_cast<uint64_t>(std::get<0>(val)), static_cast<uint64_t>(std::get<1>(val)), static_cast<uint64_t>(std::get<2>(val)));
    }
    SECTION("Scalar - double")
    {
        RunModTests(std::get<0>(val), std::get<1>(val), std::get<2>(val));
    }
    SECTION("Scalar - float")
    {
        RunModTests(static_cast<float>(std::get<0>(val)), static_cast<float>(std::get<1>(val)), static_cast<float>(std::get<2>(val)));
    }

    SECTION("Vec2 - double")
    {
        RunModTests(TRAP::Math::Vec2d(std::get<0>(val)), TRAP::Math::Vec2d(std::get<1>(val)), TRAP::Math::Vec2d(std::get<2>(val)));
        RunModTests(TRAP::Math::Vec2d(std::get<0>(val)), std::get<1>(val), TRAP::Math::Vec2d(std::get<2>(val)));
    }
    SECTION("Vec2 - float")
    {
        RunModTests(TRAP::Math::Vec2f(static_cast<float>(std::get<0>(val))), TRAP::Math::Vec2f(static_cast<float>(std::get<1>(val))), TRAP::Math::Vec2f(static_cast<float>(std::get<2>(val))));
        RunModTests(TRAP::Math::Vec2f(static_cast<float>(std::get<0>(val))), static_cast<float>(std::get<1>(val)), TRAP::Math::Vec2f(static_cast<float>(std::get<2>(val))));
    }

    SECTION("Vec3 - double")
    {
        RunModTests(TRAP::Math::Vec3d(std::get<0>(val)), TRAP::Math::Vec3d(std::get<1>(val)), TRAP::Math::Vec3d(std::get<2>(val)));
        RunModTests(TRAP::Math::Vec3d(std::get<0>(val)), std::get<1>(val), TRAP::Math::Vec3d(std::get<2>(val)));
    }
    SECTION("Vec3 - float")
    {
        RunModTests(TRAP::Math::Vec3f(static_cast<float>(std::get<0>(val))), TRAP::Math::Vec3f(static_cast<float>(std::get<1>(val))), TRAP::Math::Vec3f(static_cast<float>(std::get<2>(val))));
        RunModTests(TRAP::Math::Vec3f(static_cast<float>(std::get<0>(val))), static_cast<float>(std::get<1>(val)), TRAP::Math::Vec3f(static_cast<float>(std::get<2>(val))));
    }

    SECTION("Vec4 - double")
    {
        RunModTests(TRAP::Math::Vec4d(std::get<0>(val)), TRAP::Math::Vec4d(std::get<1>(val)), TRAP::Math::Vec4d(std::get<2>(val)));
        RunModTests(TRAP::Math::Vec4d(std::get<0>(val)), std::get<1>(val), TRAP::Math::Vec4d(std::get<2>(val)));
    }
    SECTION("Vec4 - float")
    {
        RunModTests(TRAP::Math::Vec4f(static_cast<float>(std::get<0>(val))), TRAP::Math::Vec4f(static_cast<float>(std::get<1>(val))), TRAP::Math::Vec4f(static_cast<float>(std::get<2>(val))));
        RunModTests(TRAP::Math::Vec4f(static_cast<float>(std::get<0>(val))), static_cast<float>(std::get<1>(val)), TRAP::Math::Vec4f(static_cast<float>(std::get<2>(val))));
    }
}
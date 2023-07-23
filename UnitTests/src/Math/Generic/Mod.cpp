#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T, typename X = T>
requires (std::is_arithmetic_v<T> || (TRAP::Math::IsVec<T> && std::floating_point<typename T::valueType>)) &&
         (std::is_arithmetic_v<X> || (TRAP::Math::IsVec<X> && std::floating_point<typename X::valueType>))
void RunModTests()
{
    constexpr std::array<std::tuple<T, X, T>, 3> values
    {
        std::tuple<T, X, T>{T( 1.5f), X(1.0f), T( 0.5f)},
        std::tuple<T, X, T>{T(-0.2f), X(1.0f), T(-0.2f)},
        std::tuple<T, X, T>{T( 3.0f), X(2.0f), T( 1.0f)},
    };

    for(const auto& [val1, val2, expected] : values)
    {
        if constexpr(std::is_arithmetic_v<T>)
        {
            if constexpr(std::floating_point<T>)
            {
                constexpr T Epsilon = std::numeric_limits<T>::epsilon();
                REQUIRE_THAT(TRAP::Math::Mod(val1, val2), Catch::Matchers::WithinRel(expected, Epsilon));
            }
            else if constexpr(std::integral<T>)
            {
                REQUIRE(TRAP::Math::Mod(val1, val2) == expected);
            }
        }
        else if constexpr(TRAP::Math::IsVec<T> && std::floating_point<typename T::valueType> && TRAP::Math::IsVec<X>)
        {
            constexpr typename T::valueType Epsilon = std::numeric_limits<typename T::valueType>::epsilon();
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Mod(val1, T(val2)), expected, Epsilon)));
        }
        else if constexpr(TRAP::Math::IsVec<T> && std::floating_point<typename T::valueType> && std::floating_point<X>)
        {
            constexpr typename T::valueType Epsilon = std::numeric_limits<typename T::valueType>::epsilon();
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Mod(val1, val2), expected, Epsilon)));
        }
    }
}

template<typename T>
requires std::is_arithmetic_v<T>
void RunModEdgeTests()
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    constexpr T max = std::numeric_limits<T>::max();
    constexpr T min = std::numeric_limits<T>::lowest();

    if constexpr(std::floating_point<T>)
    {
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Mod(max, T(100.0)), static_cast<T>(std::fmod(max, T(100))), Epsilon));
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Mod(min, T(-50.0)), static_cast<T>(std::fmod(min, T(-50.0))), Epsilon));

        constexpr T nan = std::numeric_limits<T>::quiet_NaN();
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Mod(T(15), T(0))));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Mod(T(2.0), nan)));
    }
    else if constexpr(std::integral<T>)
    {
        REQUIRE(TRAP::Math::Mod(max, T(100.0)) == static_cast<T>(max % T(100)));

        if constexpr(std::signed_integral<T>)
        {
            REQUIRE(TRAP::Math::Mod(min, T(-50.0)) == static_cast<T>(min % T(-50.0)));
        }
    }
}

TEST_CASE("TRAP::Math::Mod()", "[math][generic][mod]")
{
    SECTION("Scalar - int8_t")
    {
        RunModTests<int8_t>();
        RunModEdgeTests<int8_t>();
    }
    SECTION("Scalar - uint8_t")
    {
        RunModTests<uint8_t>();
        RunModEdgeTests<uint8_t>();
    }
    SECTION("Scalar - int16_t")
    {
        RunModTests<int16_t>();
        RunModEdgeTests<int16_t>();
    }
    SECTION("Scalar - uint16_t")
    {
        RunModTests<uint16_t>();
        RunModEdgeTests<uint16_t>();
    }
    SECTION("Scalar - int32_t")
    {
        RunModTests<int32_t>();
        RunModEdgeTests<int32_t>();
    }
    SECTION("Scalar - uint32_t")
    {
        RunModTests<uint32_t>();
        RunModEdgeTests<uint32_t>();
    }
    SECTION("Scalar - int64_t")
    {
        RunModTests<int64_t>();
        RunModEdgeTests<int64_t>();
    }
    SECTION("Scalar - uint64_t")
    {
        RunModTests<uint64_t>();
        RunModEdgeTests<uint64_t>();
    }
    SECTION("Scalar - double")
    {
        RunModTests<double>();
        RunModEdgeTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunModTests<float>();
        RunModEdgeTests<float>();
    }

    SECTION("Vec2 - double")
    {
        RunModTests<TRAP::Math::Vec2d>();
        RunModTests<TRAP::Math::Vec2d, double>();
    }
    SECTION("Vec2 - float")
    {
        RunModTests<TRAP::Math::Vec2f>();
        RunModTests<TRAP::Math::Vec2f, float>();
    }

    SECTION("Vec3 - double")
    {
        RunModTests<TRAP::Math::Vec3d>();
        RunModTests<TRAP::Math::Vec3d, double>();
    }
    SECTION("Vec3 - float")
    {
        RunModTests<TRAP::Math::Vec3f>();
        RunModTests<TRAP::Math::Vec3f, float>();
    }

    SECTION("Vec4 - double")
    {
        RunModTests<TRAP::Math::Vec4d>();
        RunModTests<TRAP::Math::Vec4d, double>();
    }
    SECTION("Vec4 - float")
    {
        RunModTests<TRAP::Math::Vec4f>();
        RunModTests<TRAP::Math::Vec4f, float>();
    }
}
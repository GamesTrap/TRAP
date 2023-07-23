#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::floating_point<T> || (TRAP::Math::IsVec<T> && std::floating_point<typename T::valueType>)
void RunFloorTests()
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    constexpr std::array<T, 25> values
    {
        T(0.0f), T(-0.0f), T(0.1f), T(-0.1f), T(0.5f), T(-0.5f), T(0.9f), T(-0.9f), T(1.0f), T(-1.0f), T(1.1f),
        T(1.5f), T(-1.5f), T(1.9f), T(-1.9f), T(4.2f), T(4.5f), T(4.7f), T(5.0f), T(-4.2f), T(-4.5f),
        T(-4.7f), T(-5.0f), T(42e32f), T(-42e32f)
    };

    for(const T val : values)
    {
        if constexpr(std::floating_point<T>)
        {
            REQUIRE_THAT(TRAP::Math::Floor(val), Catch::Matchers::WithinRel(std::floor(val), Epsilon));
        }
        else if constexpr(TRAP::Math::IsVec<T>)
        {
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Floor(val), T(std::floor(val[0])), Epsilon)));
        }
    }
}

template<typename T>
requires std::floating_point<T>
void RunFloorEdgeTests()
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    constexpr T max = std::numeric_limits<T>::max();
    REQUIRE_THAT(TRAP::Math::Floor(max), Catch::Matchers::WithinRel(max, Epsilon));
    constexpr T min = std::numeric_limits<T>::lowest();
    REQUIRE_THAT(TRAP::Math::Floor(min), Catch::Matchers::WithinRel(min, Epsilon));
    constexpr T nan = std::numeric_limits<T>::quiet_NaN();
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Floor(nan)));
    constexpr T inf = std::numeric_limits<T>::infinity();
    REQUIRE_THAT(TRAP::Math::Floor(inf), Catch::Matchers::WithinRel(inf, Epsilon));
    constexpr T ninf = -std::numeric_limits<T>::infinity();
    REQUIRE_THAT(TRAP::Math::Floor(ninf), Catch::Matchers::WithinRel(ninf, Epsilon));
}

TEST_CASE("TRAP::Math::Floor()", "[math][generic][floor]")
{
    SECTION("Scalar - double")
    {
        RunFloorTests<double>();
        RunFloorEdgeTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunFloorTests<float>();
        RunFloorEdgeTests<float>();
    }

    SECTION("Vec2 - double")
    {
        RunFloorTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunFloorTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - double")
    {
        RunFloorTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunFloorTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - double")
    {
        RunFloorTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunFloorTests<TRAP::Math::Vec4f>();
    }
}
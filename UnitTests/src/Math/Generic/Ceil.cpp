#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::floating_point<T> || (TRAP::Math::IsVec<T> && std::floating_point<typename T::valueType>)
void RunCeilTests()
{
    constexpr std::array<T, 24> values
    {
        static_cast<T>(0.0f), static_cast<T>(-0.0f), static_cast<T>(0.1f), static_cast<T>(-0.1f),
        static_cast<T>(0.5f), static_cast<T>(-0.5f), static_cast<T>(0.9f), static_cast<T>(-0.9f),
        static_cast<T>(1.0f), static_cast<T>(-1.0f), static_cast<T>(1.5f), static_cast<T>(-1.5f),
        static_cast<T>(1.9f), static_cast<T>(-1.9f), static_cast<T>(4.2f), static_cast<T>(4.5f),
        static_cast<T>(4.7f), static_cast<T>(5.0f), static_cast<T>(-4.2f), static_cast<T>(-4.5f),
        static_cast<T>(-4.7f), static_cast<T>(-5.0f), static_cast<T>(42e32f), static_cast<T>(-42e32f)
    };

    constexpr T Epsilon = std::numeric_limits<T>::epsilon();
    for(const T val : values)
    {
        if constexpr(std::floating_point<T>)
        {
            REQUIRE_THAT(TRAP::Math::Ceil(val), Catch::Matchers::WithinRel(std::ceil(val), Epsilon));
        }
        else if constexpr(TRAP::Math::IsVec<T>)
        {
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Ceil(val), T(std::ceil(std::get<0>(val))), Epsilon)));
        }
    }
}

template<typename T>
requires std::floating_point<T>
void RunCeilEdgeTests()
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    constexpr T max = std::numeric_limits<T>::max();
    REQUIRE_THAT(TRAP::Math::Ceil(max), Catch::Matchers::WithinRel(max, Epsilon));
    constexpr T min = std::numeric_limits<T>::lowest();
    REQUIRE_THAT(TRAP::Math::Ceil(min), Catch::Matchers::WithinRel(min, Epsilon));
    constexpr T nan = std::numeric_limits<T>::quiet_NaN();
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Ceil(nan)));
    constexpr T inf = std::numeric_limits<T>::infinity();
    REQUIRE_THAT(TRAP::Math::Ceil(inf), Catch::Matchers::WithinRel(inf, Epsilon));
    constexpr T ninf = -std::numeric_limits<T>::infinity();
    REQUIRE_THAT(TRAP::Math::Ceil(ninf), Catch::Matchers::WithinRel(ninf, Epsilon));
}

TEST_CASE("TRAP::Math::Ceil()", "[math][generic][ceil]")
{
    SECTION("Scalar - double")
    {
        RunCeilTests<double>();
        RunCeilEdgeTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunCeilTests<float>();
        RunCeilEdgeTests<float>();
    }

    SECTION("Vec2 - double")
    {
        RunCeilTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunCeilTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - double")
    {
        RunCeilTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunCeilTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - double")
    {
        RunCeilTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunCeilTests<TRAP::Math::Vec4f>();
    }
}
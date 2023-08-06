#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>
#include <array>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires TRAP::Math::IsVec<T> && std::floating_point<typename T::valueType>
void RunRefractVecTests()
{
    constexpr typename T::valueType Epsilon = std::numeric_limits<typename T::valueType>::epsilon();

    {
        constexpr T incident(TRAP::Math::Vec<4, typename T::valueType>(1.0f, 0.0f, 0.0f, 0.0f));
        constexpr T normal(TRAP::Math::Vec<4, typename T::valueType>(1.0f, 0.0f, 0.0f, 0.0f));
        constexpr typename T::valueType eta = 1.5f;
        constexpr T expected(TRAP::Math::Vec<4, typename T::valueType>(-1.0f, 0.0f, 0.0f, 0.0f));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Refract(incident, normal, eta), expected, Epsilon)));
    }
    {
        constexpr T incident(TRAP::Math::Vec<4, typename T::valueType>(0.0f, 1.0f, 0.0f, 0.0f));
        constexpr T normal(TRAP::Math::Vec<4, typename T::valueType>(1.0f, 0.0f, 0.0f, 0.0f));
        constexpr typename T::valueType eta = 1.5f;
        constexpr T expected(TRAP::Math::Vec<4, typename T::valueType>(0.0f));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Refract(incident, normal, eta), expected, Epsilon)));
    }
    {
        constexpr T incident(TRAP::Math::Vec<4, typename T::valueType>(1.0f, 1.0f, 0.0f, 0.0f));
        constexpr T normal(TRAP::Math::Vec<4, typename T::valueType>(1.0f, 1.0f, 0.0f, 0.0f));
        constexpr typename T::valueType eta = 1.5f;
        constexpr T expected(TRAP::Math::Vec<4, typename T::valueType>(-4.2838823f, -4.2838823f, 0.0f, 0.0f));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Refract(incident, normal, eta), expected, typename T::valueType(0.0000001f))));
    }
    {
        constexpr T incident(TRAP::Math::Vec<4, typename T::valueType>(-1.0f, 1.0f, 0.0f, 0.0f));
        constexpr T normal(TRAP::Math::Vec<4, typename T::valueType>(-1.0f, -1.0f, 0.0f, 0.0f));
        constexpr typename T::valueType eta = 1.5f;
        constexpr T expected(TRAP::Math::Vec<4, typename T::valueType>(0.0f));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Refract(incident, normal, eta), expected, Epsilon)));
    }
    {
        constexpr T incident(TRAP::Math::Vec<4, typename T::valueType>(0.0f, 0.0f, 1.0f, 0.0f));
        constexpr T normal(TRAP::Math::Vec<4, typename T::valueType>(1.0f, 0.0f, 0.0f, 0.0f));
        constexpr typename T::valueType eta = 1.5f;
        constexpr T expected(TRAP::Math::Vec<4, typename T::valueType>(0.0f));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Refract(incident, normal, eta), expected, Epsilon)));
    }
}

template<typename T>
requires TRAP::Math::IsVec<T> && std::floating_point<typename T::valueType>
void RunRefractVecEdgeTests()
{
    constexpr typename T::valueType Epsilon = std::numeric_limits<typename T::valueType>::epsilon();

    constexpr typename T::valueType max = std::numeric_limits<typename T::valueType>::max();
    constexpr typename T::valueType min = std::numeric_limits<typename T::valueType>::lowest();
    constexpr typename T::valueType inf = std::numeric_limits<typename T::valueType>::infinity();
    constexpr typename T::valueType ninf = -std::numeric_limits<typename T::valueType>::infinity();
    constexpr typename T::valueType nan = std::numeric_limits<typename T::valueType>::quiet_NaN();

    {
        constexpr T incident(TRAP::Math::Vec<4, typename T::valueType>(min, min, min, 0.0f));
        constexpr T normal(TRAP::Math::Vec<4, typename T::valueType>(1.0f, 1.0f, 1.0f, 0.0f));
        constexpr typename T::valueType eta = 1.5f;
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::Refract(incident, normal, eta))));
    }
    {
        constexpr T incident(TRAP::Math::Vec<4, typename T::valueType>(max, max, max, 0.0f));
        constexpr T normal(TRAP::Math::Vec<4, typename T::valueType>(1.0f, 1.0f, 1.0f, 0.0f));
        constexpr typename T::valueType eta = 1.5f;
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::Refract(incident, normal, eta))));
    }
    {
        constexpr T incident(TRAP::Math::Vec<4, typename T::valueType>(inf, inf, inf, 0.0f));
        constexpr T normal(TRAP::Math::Vec<4, typename T::valueType>(1.0f, 1.0f, 1.0f, 0.0f));
        constexpr typename T::valueType eta = 1.5f;
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::Refract(incident, normal, eta))));
    }
    {
        constexpr T incident(TRAP::Math::Vec<4, typename T::valueType>(ninf, ninf, ninf, 0.0f));
        constexpr T normal(TRAP::Math::Vec<4, typename T::valueType>(1.0f, 1.0f, 1.0f, 0.0f));
        constexpr typename T::valueType eta = 1.5f;
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::Refract(incident, normal, eta))));
    }
    {
        constexpr T incident(TRAP::Math::Vec<4, typename T::valueType>(nan, nan, nan, 0.0f));
        constexpr T normal(TRAP::Math::Vec<4, typename T::valueType>(1.0f, 1.0f, 1.0f, 0.0f));
        constexpr typename T::valueType eta = 1.5f;
        constexpr T expected(TRAP::Math::Vec<4, typename T::valueType>(0.0f));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Refract(incident, normal, eta), expected, Epsilon)));
    }
    {
        constexpr T incident(TRAP::Math::Vec<4, typename T::valueType>(1.0f, 2.0f, 3.0f, 0.0f));
        constexpr T normal(TRAP::Math::Vec<4, typename T::valueType>(nan, nan, nan, 0.0f));
        constexpr typename T::valueType eta = 1.5f;
        constexpr T expected(TRAP::Math::Vec<4, typename T::valueType>(0.0f));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Refract(incident, normal, eta), expected, Epsilon)));
    }
}

TEST_CASE("TRAP::Math::Refract()", "[math][generic][refract]")
{
    SECTION("Vec4 - double")
    {
        RunRefractVecTests<TRAP::Math::Vec4d>();
        RunRefractVecEdgeTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunRefractVecTests<TRAP::Math::Vec4f>();
        RunRefractVecEdgeTests<TRAP::Math::Vec4f>();
    }

    SECTION("Vec3 - double")
    {
        RunRefractVecTests<TRAP::Math::Vec3d>();
        RunRefractVecEdgeTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunRefractVecTests<TRAP::Math::Vec3f>();
        RunRefractVecEdgeTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec2 - double")
    {
        RunRefractVecTests<TRAP::Math::Vec2d>();
        RunRefractVecEdgeTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunRefractVecTests<TRAP::Math::Vec2f>();
        RunRefractVecEdgeTests<TRAP::Math::Vec2f>();
    }
}
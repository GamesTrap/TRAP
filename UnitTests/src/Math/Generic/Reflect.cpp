#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>
#include <array>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires TRAP::Math::IsVec<T> && std::floating_point<typename T::valueType>
consteval void RunCompileTimeReflectVecTests()
{
    constexpr typename T::valueType Epsilon = std::numeric_limits<typename T::valueType>::epsilon();

    {
        constexpr T incident(TRAP::Math::Vec<4, typename T::valueType>(1.0f, 0.0f, -1.0f, 0.0f));
        constexpr T normal(TRAP::Math::Vec<4, typename T::valueType>(0.0f, 1.0f, 0.0f, 0.0f));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Reflect(incident, normal), incident, Epsilon)));
    }
    {
        constexpr T incident(TRAP::Math::Vec<4, typename T::valueType>(0.0f, 1.0f, 0.0f, 0.0f));
        constexpr T normal(TRAP::Math::Vec<4, typename T::valueType>(1.0f, 0.0f, 0.0f, 0.0f));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Reflect(incident, normal), incident, Epsilon)));
    }
    {
        constexpr T incident(TRAP::Math::Vec<4, typename T::valueType>(0.0f, 0.0f, 0.0f, 0.0f));
        constexpr T normal(TRAP::Math::Vec<4, typename T::valueType>(1.0f, 0.0f, 0.0f, 0.0f));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Reflect(incident, normal), incident, Epsilon)));
    }
    {
        constexpr T incident(TRAP::Math::Vec<4, typename T::valueType>(0.0f, 0.0f, 1.0f, 0.0f));
        constexpr T normal(TRAP::Math::Vec<4, typename T::valueType>(0.0f, 0.0f, 1.0f, 0.0f));
        constexpr T expected(TRAP::Math::Vec<4, typename T::valueType>(0.0f, 0.0f, -1.0f, 0.0f));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Reflect(incident, normal), expected, Epsilon)));
    }
    {
        constexpr T incident(TRAP::Math::Vec<4, typename T::valueType>(1.0f, 1.0f, 0.0f, 0.0f));
        constexpr T normal(TRAP::Math::Vec<4, typename T::valueType>(0.0f, 1.0f, 0.0f, 0.0f));
        constexpr T expected(TRAP::Math::Vec<4, typename T::valueType>(1.0f, -1.0f, 0.0f, 0.0f));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Reflect(incident, normal), expected, Epsilon)));
    }
    {
        constexpr T incident(TRAP::Math::Vec<4, typename T::valueType>(1.0f, 0.0f, 0.0f, 0.0f));
        constexpr T normal(TRAP::Math::Vec<4, typename T::valueType>(0.0f, 0.0f, 1.0f, 0.0f));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Reflect(incident, normal), incident, Epsilon)));
    }
    {
        constexpr T incident(TRAP::Math::Vec<4, typename T::valueType>(1.0f, 2.0f, 3.0f, 4.0f));
        constexpr T normal(TRAP::Math::Vec<4, typename T::valueType>(0.0f, 0.0f, 0.0f, 0.0f));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Reflect(incident, normal), incident, Epsilon)));
    }
}

template<typename T>
requires TRAP::Math::IsVec<T> && std::floating_point<typename T::valueType>
void RunReflectVecEdgeTests()
{
    constexpr typename T::valueType max = std::numeric_limits<typename T::valueType>::max();
    constexpr typename T::valueType min = std::numeric_limits<typename T::valueType>::lowest();
    constexpr typename T::valueType inf = std::numeric_limits<typename T::valueType>::infinity();
    constexpr typename T::valueType ninf = -std::numeric_limits<typename T::valueType>::infinity();
    constexpr typename T::valueType nan = std::numeric_limits<typename T::valueType>::quiet_NaN();

    {
        constexpr T incident(TRAP::Math::Vec<4, typename T::valueType>(max, 0.0f, 0.0f, 0.0f));
        constexpr T normal(TRAP::Math::Vec<4, typename T::valueType>(max, 0.0f, 1.0f, 0.0f));
        REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(TRAP::Math::Reflect(incident, normal))));
    }
    {
        constexpr T incident(TRAP::Math::Vec<4, typename T::valueType>(min, 0.0f, 0.0f, 0.0f));
        constexpr T normal(TRAP::Math::Vec<4, typename T::valueType>(min, 0.0f, 1.0f, 0.0f));
        REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(TRAP::Math::Reflect(incident, normal))));
    }
    {
        constexpr T incident(TRAP::Math::Vec<4, typename T::valueType>(inf, 0.0f, 0.0f, 0.0f));
        constexpr T normal(TRAP::Math::Vec<4, typename T::valueType>(inf, 0.0f, 1.0f, 0.0f));
        REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(TRAP::Math::Reflect(incident, normal))));
    }
    {
        constexpr T incident(TRAP::Math::Vec<4, typename T::valueType>(ninf, 0.0f, 0.0f, 0.0f));
        constexpr T normal(TRAP::Math::Vec<4, typename T::valueType>(ninf, 0.0f, 1.0f, 0.0f));
        REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(TRAP::Math::Reflect(incident, normal))));
    }
    {
        constexpr T incident(TRAP::Math::Vec<4, typename T::valueType>(nan, 0.0f, 0.0f, 0.0f));
        constexpr T normal(TRAP::Math::Vec<4, typename T::valueType>(nan, 0.0f, 1.0f, 0.0f));
        REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(TRAP::Math::Reflect(incident, normal))));
    }
}

TEST_CASE("TRAP::Math::Reflect()", "[math][generic][reflect]")
{
    SECTION("Vec4 - double")
    {
        RunCompileTimeReflectVecTests<TRAP::Math::Vec4d>();
        RunReflectVecEdgeTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunCompileTimeReflectVecTests<TRAP::Math::Vec4f>();
        RunReflectVecEdgeTests<TRAP::Math::Vec4f>();
    }

    SECTION("Vec3 - double")
    {
        RunCompileTimeReflectVecTests<TRAP::Math::Vec3d>();
        RunReflectVecEdgeTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunCompileTimeReflectVecTests<TRAP::Math::Vec3f>();
        RunReflectVecEdgeTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec2 - double")
    {
        RunCompileTimeReflectVecTests<TRAP::Math::Vec2d>();
        RunReflectVecEdgeTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunCompileTimeReflectVecTests<TRAP::Math::Vec2f>();
        RunReflectVecEdgeTests<TRAP::Math::Vec2f>();
    }
}
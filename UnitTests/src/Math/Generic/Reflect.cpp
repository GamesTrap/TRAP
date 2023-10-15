#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>
#include <array>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    consteval void RunCompileTimeReflectVecTests()
    {
        constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

        {
            constexpr T incident(TRAP::Math::Vec<4, typename T::value_type>(1.0f, 0.0f, -1.0f, 0.0f));
            constexpr T normal(TRAP::Math::Vec<4, typename T::value_type>(0.0f, 1.0f, 0.0f, 0.0f));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Reflect(incident, normal), incident, Epsilon)));
        }
        {
            constexpr T incident(TRAP::Math::Vec<4, typename T::value_type>(0.0f, 1.0f, 0.0f, 0.0f));
            constexpr T normal(TRAP::Math::Vec<4, typename T::value_type>(1.0f, 0.0f, 0.0f, 0.0f));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Reflect(incident, normal), incident, Epsilon)));
        }
        {
            constexpr T incident(TRAP::Math::Vec<4, typename T::value_type>(0.0f, 0.0f, 0.0f, 0.0f));
            constexpr T normal(TRAP::Math::Vec<4, typename T::value_type>(1.0f, 0.0f, 0.0f, 0.0f));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Reflect(incident, normal), incident, Epsilon)));
        }
        {
            constexpr T incident(TRAP::Math::Vec<4, typename T::value_type>(0.0f, 0.0f, 1.0f, 0.0f));
            constexpr T normal(TRAP::Math::Vec<4, typename T::value_type>(0.0f, 0.0f, 1.0f, 0.0f));
            constexpr T expected(TRAP::Math::Vec<4, typename T::value_type>(0.0f, 0.0f, -1.0f, 0.0f));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Reflect(incident, normal), expected, Epsilon)));
        }
        {
            constexpr T incident(TRAP::Math::Vec<4, typename T::value_type>(1.0f, 1.0f, 0.0f, 0.0f));
            constexpr T normal(TRAP::Math::Vec<4, typename T::value_type>(0.0f, 1.0f, 0.0f, 0.0f));
            constexpr T expected(TRAP::Math::Vec<4, typename T::value_type>(1.0f, -1.0f, 0.0f, 0.0f));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Reflect(incident, normal), expected, Epsilon)));
        }
        {
            constexpr T incident(TRAP::Math::Vec<4, typename T::value_type>(1.0f, 0.0f, 0.0f, 0.0f));
            constexpr T normal(TRAP::Math::Vec<4, typename T::value_type>(0.0f, 0.0f, 1.0f, 0.0f));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Reflect(incident, normal), incident, Epsilon)));
        }
        {
            constexpr T incident(TRAP::Math::Vec<4, typename T::value_type>(1.0f, 2.0f, 3.0f, 4.0f));
            constexpr T normal(TRAP::Math::Vec<4, typename T::value_type>(0.0f, 0.0f, 0.0f, 0.0f));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Reflect(incident, normal), incident, Epsilon)));
        }
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    void RunReflectVecEdgeTests()
    {
        static constexpr typename T::value_type max = std::numeric_limits<typename T::value_type>::max();
        static constexpr typename T::value_type min = std::numeric_limits<typename T::value_type>::lowest();
        static constexpr typename T::value_type inf = std::numeric_limits<typename T::value_type>::infinity();
        static constexpr typename T::value_type ninf = -std::numeric_limits<typename T::value_type>::infinity();
        static constexpr typename T::value_type nan = std::numeric_limits<typename T::value_type>::quiet_NaN();

        {
            static constexpr T incident(TRAP::Math::Vec<4, typename T::value_type>(max, 0.0f, 0.0f, 0.0f));
            static constexpr T normal(TRAP::Math::Vec<4, typename T::value_type>(max, 0.0f, 1.0f, 0.0f));
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(TRAP::Math::Reflect(incident, normal))));
        }
        {
            static constexpr T incident(TRAP::Math::Vec<4, typename T::value_type>(min, 0.0f, 0.0f, 0.0f));
            static constexpr T normal(TRAP::Math::Vec<4, typename T::value_type>(min, 0.0f, 1.0f, 0.0f));
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(TRAP::Math::Reflect(incident, normal))));
        }
        {
            static constexpr T incident(TRAP::Math::Vec<4, typename T::value_type>(inf, 0.0f, 0.0f, 0.0f));
            static constexpr T normal(TRAP::Math::Vec<4, typename T::value_type>(inf, 0.0f, 1.0f, 0.0f));
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(TRAP::Math::Reflect(incident, normal))));
        }
        {
            static constexpr T incident(TRAP::Math::Vec<4, typename T::value_type>(ninf, 0.0f, 0.0f, 0.0f));
            static constexpr T normal(TRAP::Math::Vec<4, typename T::value_type>(ninf, 0.0f, 1.0f, 0.0f));
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(TRAP::Math::Reflect(incident, normal))));
        }
        {
            static constexpr T incident(TRAP::Math::Vec<4, typename T::value_type>(nan, 0.0f, 0.0f, 0.0f));
            static constexpr T normal(TRAP::Math::Vec<4, typename T::value_type>(nan, 0.0f, 1.0f, 0.0f));
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(TRAP::Math::Reflect(incident, normal))));
        }
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
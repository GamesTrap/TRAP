#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>
#include <array>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires TRAP::Math::IsVec2<T> && std::floating_point<typename T::value_type>
void RunDistanceVec2Tests()
{
    static constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

    {
        static constexpr T p0(1.0f, 0.0f);
        static constexpr typename T::value_type expected(0.0f);
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Distance(p0, p0), expected, Epsilon));
    }
    {
        static constexpr T p0(1.0f, 2.0f);
        static constexpr T p1(5.0f, 6.0f);
        static constexpr typename T::value_type expected(5.6568542494923806f);
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Distance(p0, p1), expected, typename T::value_type(0.000001f)));
    }
    {
        static constexpr T p0(0.0f, 0.0f);
        static constexpr typename T::value_type expected(0.0f);
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Distance(p0, p0), expected, Epsilon));
    }
    {
        static constexpr T p0(1.0f, 2.0f);
        static constexpr T p1(-1.0f, -2.0f);
        static constexpr typename T::value_type expected(4.4721359549995796f);
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Distance(p0, p1), expected, typename T::value_type(0.000001f)));
    }
}

template<typename T>
requires TRAP::Math::IsVec3<T> && std::floating_point<typename T::value_type>
void RunDistanceVec3Tests()
{
    static constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

    {
        static constexpr T p0(1.0f, 0.0f, 0.0f);
        static constexpr typename T::value_type expected(0.0f);
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Distance(p0, p0), expected, Epsilon));
    }
    {
        static constexpr T p0(1.0f, 2.0f, 3.0f);
        static constexpr T p1(5.0f, 6.0f, 7.0f);
        static constexpr typename T::value_type expected(6.9282032302755088f);
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Distance(p0, p1), expected, typename T::value_type(0.000001f)));
    }
    {
        static constexpr T p0(0.0f, 0.0f, 0.0f);
        static constexpr typename T::value_type expected(0.0f);
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Distance(p0, p0), expected, Epsilon));
    }
    {
        static constexpr T p0(1.0f, 2.0f, 3.0f);
        static constexpr T p1(-1.0f, -2.0f, -3.0f);
        static constexpr typename T::value_type expected(7.48331499f);
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Distance(p0, p1), expected, typename T::value_type(0.000001f)));
    }
}

template<typename T>
requires TRAP::Math::IsVec4<T> && std::floating_point<typename T::value_type>
void RunDistanceVec4Tests()
{
    static constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

    {
        static constexpr T p0(1.0f, 0.0f, 0.0f, 0.0f);
        static constexpr typename T::value_type expected(0.0f);
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Distance(p0, p0), expected, Epsilon));
    }
    {
        static constexpr T p0(1.0f, 2.0f, 3.0f, 4.0f);
        static constexpr T p1(5.0f, 6.0f, 7.0f, 8.0f);
        static constexpr typename T::value_type expected(8.0f);
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Distance(p0, p1), expected, Epsilon));
    }
    {
        static constexpr T p0(0.0f, 0.0f, 0.0f, 0.0f);
        static constexpr typename T::value_type expected(0.0f);
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Distance(p0, p0), expected, Epsilon));
    }
    {
        static constexpr T p0(1.0f, 2.0f, 3.0f, 4.0f);
        static constexpr T p1(-1.0f, -2.0f, -3.0f, -4.0f);
        static constexpr typename T::value_type expected(10.954451150103322f);
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Distance(p0, p1), expected, typename T::value_type(0.000001f)));
    }
}

template<typename T>
requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
void RunDistanceVecEdgeTests()
{
    static constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

    static constexpr typename T::value_type max = std::numeric_limits<typename T::value_type>::max();
    static constexpr typename T::value_type min = std::numeric_limits<typename T::value_type>::min();
    static constexpr typename T::value_type nan = std::numeric_limits<typename T::value_type>::quiet_NaN();
    static constexpr typename T::value_type inf = std::numeric_limits<typename T::value_type>::infinity();
    static constexpr typename T::value_type ninf = -std::numeric_limits<typename T::value_type>::infinity();

    {
        static constexpr T p0(TRAP::Math::Vec<4, typename T::value_type>(0.0f, 0.0f, 0.0f, 0.0f));
        static constexpr T p1(TRAP::Math::Vec<4, typename T::value_type>(max, 0.0f, 0.0f, 0.0f));
        REQUIRE(TRAP::Math::IsInf(TRAP::Math::Distance(p0, p1)));
    }
    {
        static constexpr T p0(TRAP::Math::Vec<4, typename T::value_type>(0.0f, 0.0f, 0.0f, 0.0f));
        static constexpr T p1(TRAP::Math::Vec<4, typename T::value_type>(min, 0.0f, 0.0f, 0.0f));
        static constexpr typename T::value_type expected(0.0f);
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Distance(p0, p1), expected, Epsilon));
    }
    {
        static constexpr T p0(TRAP::Math::Vec<4, typename T::value_type>(0.0f, 0.0f, 0.0f, 0.0f));
        static constexpr T p1(TRAP::Math::Vec<4, typename T::value_type>(nan, 0.0f, 0.0f, 0.0f));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Distance(p0, p1)));
    }
    {
        static constexpr T p0(TRAP::Math::Vec<4, typename T::value_type>(0.0f, 0.0f, 0.0f, 0.0f));
        static constexpr T p1(TRAP::Math::Vec<4, typename T::value_type>(inf, 0.0f, 0.0f, 0.0f));
        REQUIRE(TRAP::Math::IsInf(TRAP::Math::Distance(p0, p1)));
    }
    {
        static constexpr T p0(TRAP::Math::Vec<4, typename T::value_type>(0.0f, 0.0f, 0.0f, 0.0f));
        static constexpr T p1(TRAP::Math::Vec<4, typename T::value_type>(ninf, 0.0f, 0.0f, 0.0f));
        REQUIRE(TRAP::Math::IsInf(TRAP::Math::Distance(p0, p1)));
    }
}

TEST_CASE("TRAP::Math::Distance()", "[math][generic][distance]")
{
    SECTION("Vec2 - double")
    {
        RunDistanceVec2Tests<TRAP::Math::Vec2d>();
        RunDistanceVecEdgeTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunDistanceVec2Tests<TRAP::Math::Vec2f>();
        RunDistanceVecEdgeTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - double")
    {
        RunDistanceVec3Tests<TRAP::Math::Vec3d>();
        RunDistanceVecEdgeTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunDistanceVec3Tests<TRAP::Math::Vec3f>();
        RunDistanceVecEdgeTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - double")
    {
        RunDistanceVec4Tests<TRAP::Math::Vec4d>();
        RunDistanceVecEdgeTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunDistanceVec4Tests<TRAP::Math::Vec4f>();
        RunDistanceVecEdgeTests<TRAP::Math::Vec4f>();
    }
}
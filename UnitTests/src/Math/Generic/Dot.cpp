#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>
#include <array>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires TRAP::Math::IsVec4<T> && std::floating_point<typename T::value_type>
consteval void RunCompileTimeDotVec4Tests()
{
    constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

    {
        constexpr T x(1.0f, 2.0f, 3.0f, 4.0f);
        constexpr T y(5.0f, 6.0f, 7.0f, 8.0f);
        static_assert(TRAP::Math::Equal(TRAP::Math::Dot(x, y), typename T::value_type(70.0f), Epsilon));
    }
    {
        constexpr T x(0.0f, 0.0f, 0.0f, 0.0f);
        constexpr T y(5.0f, 6.0f, 7.0f, 8.0f);
        static_assert(TRAP::Math::Equal(TRAP::Math::Dot(x, y), typename T::value_type(0.0f), Epsilon));
    }
    {
        constexpr T x(1.0f, 2.0f, 3.0f, 4.0f);
        constexpr T y(2.0f, 4.0f, 6.0f, 8.0f);
        static_assert(TRAP::Math::Equal(TRAP::Math::Dot(x, y), typename T::value_type(60.0f), Epsilon));
    }
    {
        constexpr T x(1.0f, 0.0f, 0.0f, 0.0f);
        constexpr T y(0.0f, 1.0f, 0.0f, 0.0f);
        static_assert(TRAP::Math::Equal(TRAP::Math::Dot(x, y), typename T::value_type(0.0f), Epsilon));
    }
    {
        constexpr T x(-1.0f, -2.0f, -3.0f, -4.0f);
        constexpr T y(5.0f, 6.0f, 7.0f, 8.0f);
        static_assert(TRAP::Math::Equal(TRAP::Math::Dot(x, y), typename T::value_type(-70.0f), Epsilon));
    }
    {
        constexpr T x(-1.0f, 2.0f, -3.0f, 4.0f);
        constexpr T y(5.0f, -6.0f, 7.0f, -8.0f);
        static_assert(TRAP::Math::Equal(TRAP::Math::Dot(x, y), typename T::value_type(-70.0f), Epsilon));
    }
}

template<typename T>
requires TRAP::Math::IsVec3<T> && std::floating_point<typename T::value_type>
consteval void RunCompileTimeDotVec3Tests()
{
    constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

    {
        constexpr T x(1.0f, 2.0f, 3.0f);
        constexpr T y(5.0f, 6.0f, 7.0f);
        static_assert(TRAP::Math::Equal(TRAP::Math::Dot(x, y), typename T::value_type(38.0f), Epsilon));
    }
    {
        constexpr T x(0.0f, 0.0f, 0.0f);
        constexpr T y(5.0f, 6.0f, 7.0f);
        static_assert(TRAP::Math::Equal(TRAP::Math::Dot(x, y), typename T::value_type(0.0f), Epsilon));
    }
    {
        constexpr T x(1.0f, 2.0f, 3.0f);
        constexpr T y(2.0f, 4.0f, 6.0f);
        static_assert(TRAP::Math::Equal(TRAP::Math::Dot(x, y), typename T::value_type(28.0f), Epsilon));
    }
    {
        constexpr T x(1.0f, 0.0f, 0.0f);
        constexpr T y(0.0f, 1.0f, 0.0f);
        static_assert(TRAP::Math::Equal(TRAP::Math::Dot(x, y), typename T::value_type(0.0f), Epsilon));
    }
    {
        constexpr T x(-1.0f, -2.0f, -3.0f);
        constexpr T y(5.0f, 6.0f, 7.0f);
        static_assert(TRAP::Math::Equal(TRAP::Math::Dot(x, y), typename T::value_type(-38.0f), Epsilon));
    }
    {
        constexpr T x(-1.0f, 2.0f, -3.0f);
        constexpr T y(5.0f, -6.0f, 7.0f);
        static_assert(TRAP::Math::Equal(TRAP::Math::Dot(x, y), typename T::value_type(-38.0f), Epsilon));
    }
}

template<typename T>
requires TRAP::Math::IsVec2<T> && std::floating_point<typename T::value_type>
consteval void RunCompileTimeDotVec2Tests()
{
    constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

    {
        constexpr T x(1.0f, 2.0f);
        constexpr T y(5.0f, 6.0f);
        static_assert(TRAP::Math::Equal(TRAP::Math::Dot(x, y), typename T::value_type(17.0f), Epsilon));
    }
    {
        constexpr T x(0.0f, 0.0f);
        constexpr T y(5.0f, 6.0f);
        static_assert(TRAP::Math::Equal(TRAP::Math::Dot(x, y), typename T::value_type(0.0f), Epsilon));
    }
    {
        constexpr T x(1.0f, 2.0f);
        constexpr T y(2.0f, 4.0f);
        static_assert(TRAP::Math::Equal(TRAP::Math::Dot(x, y), typename T::value_type(10.0f), Epsilon));
    }
    {
        constexpr T x(1.0f, 0.0f);
        constexpr T y(0.0f, 1.0f);
        static_assert(TRAP::Math::Equal(TRAP::Math::Dot(x, y), typename T::value_type(0.0f), Epsilon));
    }
    {
        constexpr T x(-1.0f, -2.0f);
        constexpr T y(5.0f, 6.0f);
        static_assert(TRAP::Math::Equal(TRAP::Math::Dot(x, y), typename T::value_type(-17.0f), Epsilon));
    }
    {
        constexpr T x(-1.0f, 2.0f);
        constexpr T y(5.0f, -6.0f);
        static_assert(TRAP::Math::Equal(TRAP::Math::Dot(x, y), typename T::value_type(-17.0f), Epsilon));
    }
}

template<typename T>
requires TRAP::Math::IsVec4<T> && std::floating_point<typename T::value_type>
void RunDotVecEdgeTests()
{
    constexpr typename T::value_type max = std::numeric_limits<typename T::value_type>::max();
    constexpr typename T::value_type min = std::numeric_limits<typename T::value_type>::lowest();
    constexpr typename T::value_type inf = std::numeric_limits<typename T::value_type>::infinity();
    constexpr typename T::value_type ninf = -std::numeric_limits<typename T::value_type>::infinity();
    constexpr typename T::value_type nan = -std::numeric_limits<typename T::value_type>::quiet_NaN();

    {
        constexpr T x(max, 2.0f, 3.0f, 4.0f);
        constexpr T y(5.0f, 6.0f, 7.0f, 8.0f);
        REQUIRE(TRAP::Math::IsInf(TRAP::Math::Dot(x, y)));
    }
    {
        constexpr T x(min, 2.0f, 3.0f, 4.0f);
        constexpr T y(5.0f, 6.0f, 7.0f, 8.0f);
        REQUIRE(TRAP::Math::IsInf(TRAP::Math::Dot(x, y)));
    }
    {
        constexpr T x(inf, 2.0f, 3.0f, 4.0f);
        constexpr T y(5.0f, 6.0f, 7.0f, 8.0f);
        static_assert(TRAP::Math::IsInf(TRAP::Math::Dot(x, y)));
    }
    {
        constexpr T x(ninf, 2.0f, 3.0f, 4.0f);
        constexpr T y(5.0f, 6.0f, 7.0f, 8.0f);
        static_assert(TRAP::Math::IsInf(TRAP::Math::Dot(x, y)));
    }
    {
        constexpr T x(nan, 2.0f, 3.0f, 4.0f);
        constexpr T y(5.0f, 6.0f, 7.0f, 8.0f);
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Dot(x, y)));
    }
}

template<typename T>
requires TRAP::Math::IsQuat<T>
consteval void RunCompileTimeDotQuatTests()
{
    constexpr typename T::value_type Epsilon = typename T::value_type(0.001f);

    {
        constexpr T q1(0.5f, 0.2f, 0.7f, 0.9f);
        constexpr T q2(0.3f, 0.4f, 0.1f, 0.6f);
        static_assert(TRAP::Math::Equal(TRAP::Math::Dot(q1, q2), typename T::value_type(0.84f), Epsilon));
    }
    {
        constexpr T q1(0.0f, 0.0f, 0.0f, 0.0f);
        constexpr T q2(0.3f, 0.4f, 0.1f, 0.6f);
        static_assert(TRAP::Math::Equal(TRAP::Math::Dot(q1, q2), typename T::value_type(0.0f), Epsilon));
    }
    {
        constexpr T q1(0.5f, 0.2f, 0.7f, 0.9f);
        constexpr T q2(1.0f, 0.4f, 1.4f, 1.8f);
        static_assert(TRAP::Math::Equal(TRAP::Math::Dot(q1, q2), typename T::value_type(3.18f), Epsilon));
    }
    {
        constexpr T q1(-0.5f, -0.2f, -0.7f, -0.9f);
        constexpr T q2(0.3f, 0.4f, 0.1f, 0.6f);
        static_assert(TRAP::Math::Equal(TRAP::Math::Dot(q1, q2), typename T::value_type(-0.84f), Epsilon));
    }
    {
        constexpr T q1(-0.5f, 0.2f, -0.7f, 0.9f);
        constexpr T q2(0.3f, -0.4f, 0.1f, -0.6f);
        static_assert(TRAP::Math::Equal(TRAP::Math::Dot(q1, q2), typename T::value_type(-0.84f), Epsilon));
    }
    {
        constexpr T q1(1.0f, 0.0f, 0.0f, 0.0f);
        constexpr T q2(0.3f, 0.4f, 0.1f, 0.6f);
        static_assert(TRAP::Math::Equal(TRAP::Math::Dot(q1, q2), typename T::value_type(0.3f), Epsilon));
    }
    {
        constexpr T q1(1.0f, 0.0f, 0.0f, 0.0f);
        constexpr T q2(1.0f, 0.0f, 0.0f, 0.0f);
        static_assert(TRAP::Math::Equal(TRAP::Math::Dot(q1, q2), typename T::value_type(1.0f), Epsilon));
    }
}

TEST_CASE("TRAP::Math::Dot()", "[math][generic][dot]")
{
    SECTION("Vec2 - double")
    {
        RunCompileTimeDotVec2Tests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunCompileTimeDotVec2Tests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - double")
    {
        RunCompileTimeDotVec3Tests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunCompileTimeDotVec3Tests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - double")
    {
        RunCompileTimeDotVec4Tests<TRAP::Math::Vec4d>();
        RunDotVecEdgeTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunCompileTimeDotVec4Tests<TRAP::Math::Vec4f>();
        RunDotVecEdgeTests<TRAP::Math::Vec4f>();
    }

    SECTION("Quat - double")
    {
        RunCompileTimeDotQuatTests<TRAP::Math::Quatd>();
    }
    SECTION("Quat - float")
    {
        RunCompileTimeDotQuatTests<TRAP::Math::Quatf>();
    }
}
#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>
#include <array>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::floating_point<T>
void RunInverseSqrtTests()
{
    constexpr std::array<std::tuple<T, T>, 4> values
    {
        std::tuple(T(0.5f), T(1.41421f)),
        std::tuple(T(1.5f), T(0.816497f)),
        std::tuple(T(2.0f), T(0.707107f)),
        std::tuple(T(41.5f), T(0.15523f))
    };

    for(const auto& [x, expected] : values)
    {
        REQUIRE(TRAP::Math::Equal(TRAP::Math::InverseSqrt(x), expected, T(0.00001f)));
    }
}

template<typename T>
requires TRAP::Math::IsVec<T> && std::floating_point<typename T::valueType>
void RunInverseSqrtVecTests()
{
    REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::InverseSqrt(T(16.0f)) * TRAP::Math::Sqrt(T(16.0f)), T(1.0f), T(0.01f))));
}

template<typename T>
requires std::floating_point<T>
void RunInverseSqrtEdgeTests()
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    constexpr T nan = std::numeric_limits<T>::quiet_NaN();
    constexpr T inf = std::numeric_limits<T>::infinity();
    constexpr T ninf = -std::numeric_limits<T>::infinity();
    constexpr T min = std::numeric_limits<T>::lowest();

    REQUIRE(TRAP::Math::IsInf(TRAP::Math::InverseSqrt(0.0f)));
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::InverseSqrt(-1.0f)));
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::InverseSqrt(min)));
    REQUIRE(TRAP::Math::Equal(TRAP::Math::InverseSqrt(inf), T(0.0f), Epsilon));
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::InverseSqrt(ninf)));
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::InverseSqrt(nan)));
}

TEST_CASE("TRAP::Math::InverseSqrt()", "[math][generic][inversesqrt]")
{
    SECTION("Scalar - double")
    {
        RunInverseSqrtTests<double>();
        RunInverseSqrtEdgeTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunInverseSqrtTests<float>();
        RunInverseSqrtEdgeTests<float>();
    }

    SECTION("Vec2 - double")
    {
        RunInverseSqrtVecTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunInverseSqrtVecTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - double")
    {
        RunInverseSqrtVecTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunInverseSqrtVecTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - double")
    {
        RunInverseSqrtVecTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunInverseSqrtVecTests<TRAP::Math::Vec4f>();
    }
}
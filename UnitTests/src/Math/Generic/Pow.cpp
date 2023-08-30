#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>
#include <array>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::floating_point<T>
void RunPowTests()
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    constexpr std::array<std::tuple<T, T, T>, 4> values
    {
        std::tuple(T(0.5f), T(2.0f), T(0.25f)),
        std::tuple(T(1.5f), T(1.0f), T(1.5f)),
        std::tuple(T(2.0f), T(1.0f), T(2.0f)),
        std::tuple(T(2.0f), T(2.0f), T(4.0f)),
    };

    for(const auto& [base, exponent, expected] : values)
    {
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Pow(base, exponent), expected, Epsilon));
    }
}

template<typename T>
requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
void RunPowVecTests()
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();
    REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Pow(T(2.0f), T(2.0f)), T(4.0f), Epsilon)));
}

template<typename T>
requires TRAP::Math::IsQuat<T>
void RunPowQuatTests()
{
    constexpr typename T::value_type Epsilon = typename T::value_type(0.001f);

    const T q(TRAP::Math::Vec<3, typename T::value_type>(1.0f, 0.0f, 0.0f), TRAP::Math::Vec<3, typename T::value_type>(0.0f, 1.0f, 0.0f));

    {
        const typename T::value_type one = typename T::value_type(1.0f);
        const T p = TRAP::Math::Pow(q, one);
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(q, p, Epsilon)));
    }
    {
        const typename T::value_type two = typename T::value_type(2.0f);
        const T p = TRAP::Math::Pow(q, two);
        const T r = q * q;
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(p, r, Epsilon)));
    }
}

template<typename T>
requires std::floating_point<T>
void RunPowEdgeTests()
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    constexpr T nan = std::numeric_limits<T>::quiet_NaN();
    constexpr T inf = std::numeric_limits<T>::infinity();

    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Pow(nan, T(2.0f))));
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Pow(T(2.0f), nan)));
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Pow(nan, nan)));
    REQUIRE(TRAP::Math::IsInf(TRAP::Math::Pow(inf, T(2.0f))));
    REQUIRE(TRAP::Math::IsInf(TRAP::Math::Pow(-inf, T(2.0f))));
    REQUIRE(TRAP::Math::Equal(TRAP::Math::Pow(inf, T(-2.0f)), T(0.0f), Epsilon));
    REQUIRE(TRAP::Math::Equal(TRAP::Math::Pow(inf, T(0.0f)), T(1.0f), Epsilon));
}

TEST_CASE("TRAP::Math::Pow()", "[math][generic][pow]")
{
    SECTION("Scalar - double")
    {
        RunPowTests<double>();
        RunPowEdgeTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunPowTests<float>();
        RunPowEdgeTests<float>();
    }

    SECTION("Vec2 - double")
    {
        RunPowVecTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunPowVecTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - double")
    {
        RunPowVecTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunPowVecTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - double")
    {
        RunPowVecTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunPowVecTests<TRAP::Math::Vec4f>();
    }

    SECTION("Quat - double")
    {
        RunPowQuatTests<TRAP::Math::Quatd>();
    }
    SECTION("Quat - float")
    {
        RunPowQuatTests<TRAP::Math::Quatf>();
    }
}
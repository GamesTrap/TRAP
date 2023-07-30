#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>
#include <array>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::floating_point<T>
void RunExpTests()
{
    const std::array<std::tuple<T, T>, 9> values
    {
        std::tuple(T(-40.0f), T(0.0f)),
        std::tuple(T(-4.0f), T(0.0183156393f)),
        std::tuple(T(0.0001f), T(1.0001000049976403f)),
        std::tuple(T(1.0f), T(2.71828175f)),
        std::tuple(T(1.75f), T(5.75460291f)),
        std::tuple(T(1.9991f), T(7.382409f)),
        std::tuple(T(2.1f), T(8.166169f)),
        std::tuple(T(4.0f), T(54.598150033144236f)),
        std::tuple(T(0.0f), T(1.0f)),
    };

    for(const auto& [num, expected] : values)
    {
        const T res = TRAP::Math::Exp(num);
        REQUIRE(TRAP::Math::Equal(res, expected, T(0.00001f)));
    }
}

template<typename T>
requires TRAP::Math::IsVec<T> && std::floating_point<typename T::valueType>
void RunExpVecTests()
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();
    REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Exp(T(1.0f)), T(TRAP::Math::e<typename T::valueType>()), Epsilon)));
}

template<typename T>
requires TRAP::Math::IsQuat<T>
void RunExpQuatTests()
{
    constexpr typename T::valueType Epsilon = std::numeric_limits<typename T::valueType>::epsilon();

    const T q(typename T::valueType(1.0f), typename T::valueType(0.0f), typename T::valueType(0.0f), typename T::valueType(0.0f));
    const T res(typename T::valueType(0.0f), typename T::valueType(0.0f), typename T::valueType(0.0f), typename T::valueType(0.0f));
    REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Exp(q), res, Epsilon)));

    const T q2(typename T::valueType(0.0f), typename T::valueType(1.0f), typename T::valueType(0.0f), typename T::valueType(0.0f));
    const T res2(TRAP::Math::Cos(typename T::valueType(1.0f)), TRAP::Math::Sin(typename T::valueType(1.0f)), typename T::valueType(0.0f), typename T::valueType(0.0f));
    REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Exp(q2), res2, Epsilon)));

    const T q3(typename T::valueType(0.0f), typename T::valueType(0.0f), typename T::valueType(1.0f), typename T::valueType(0.0f));
    const T res3(TRAP::Math::Cos(typename T::valueType(1.0f)), typename T::valueType(0.0f), TRAP::Math::Sin(typename T::valueType(1.0f)), typename T::valueType(0.0f));
    REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Exp(q3), res3, Epsilon)));

    const T q4(typename T::valueType(0.0f), typename T::valueType(0.0f), typename T::valueType(0.0f), typename T::valueType(1.0f));
    const T res4(TRAP::Math::Cos(typename T::valueType(1.0f)), typename T::valueType(0.0f), typename T::valueType(0.0f), TRAP::Math::Sin(typename T::valueType(1.0f)));
    REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Exp(q4), res4, Epsilon)));

    const T q6(typename T::valueType(0.0f), typename T::valueType(0.0f), typename T::valueType(0.0f), typename T::valueType(0.0f));
    REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Exp(q6), q6, Epsilon)));
}

template<typename T>
requires std::floating_point<T>
void RunExpEdgeTests()
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    constexpr T nan = std::numeric_limits<T>::quiet_NaN();
    constexpr T inf = std::numeric_limits<T>::infinity();
    constexpr T ninf = -std::numeric_limits<T>::infinity();
    constexpr T max = std::numeric_limits<T>::max();
    constexpr T min = std::numeric_limits<T>::lowest();

    REQUIRE(TRAP::Math::Equal(TRAP::Math::Exp(ninf), T(0.0f), Epsilon));
    REQUIRE(TRAP::Math::IsInf(TRAP::Math::Exp(inf)));
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Exp(nan)));
    REQUIRE(TRAP::Math::IsInf(TRAP::Math::Exp(max)));
    REQUIRE(TRAP::Math::Equal(TRAP::Math::Exp(min), T(0.0f), Epsilon));
}

TEST_CASE("TRAP::Math::Exp()", "[math][generic][exp]")
{
    SECTION("Scalar - double")
    {
        RunExpTests<double>();
        RunExpEdgeTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunExpTests<float>();
        RunExpEdgeTests<float>();
    }

    SECTION("Vec2 - double")
    {
        RunExpVecTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunExpVecTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - double")
    {
        RunExpVecTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunExpVecTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - double")
    {
        RunExpVecTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunExpVecTests<TRAP::Math::Vec4f>();
    }

    SECTION("Quat - double")
    {
        RunExpQuatTests<TRAP::Math::Quatd>();
    }
    SECTION("Quat - float")
    {
        RunExpQuatTests<TRAP::Math::Quatf>();
    }
}
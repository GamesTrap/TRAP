#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>
#include <array>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires ((TRAP::Math::IsMat3<T> || TRAP::Math::IsMat4<T> || TRAP::Math::IsQuat<T>) && std::floating_point<typename T::valueType>)
consteval void RunCompileTimeInverseTests()
{
    constexpr typename T::valueType Epsilon = std::numeric_limits<typename T::valueType>::epsilon();

    if constexpr(TRAP::Math::IsMat<T>)
    {
        {
            constexpr T a(1.0f);
            constexpr T b = TRAP::Math::Inverse(a);
            constexpr T i = a * b;
            static_assert(TRAP::Math::All(TRAP::Math::Equal(i, a, Epsilon)));
        }
        {
            constexpr T a(TRAP::Math::tMat4<typename T::valueType>(0.6f, 0.2f, 0.3f, 0.4f, 0.2f, 0.7f, 0.5f, 0.3f, 0.3f, 0.5f, 0.7f, 0.2f, 0.4f, 0.3f, 0.2f, 0.6f));
            constexpr T b = TRAP::Math::Inverse(a);
            constexpr T i = a * b;
            static_assert(TRAP::Math::All(TRAP::Math::Equal(T(1.0f), i, typename T::valueType(0.000001f))));
        }
    }
    else if constexpr(TRAP::Math::IsQuat<T>)
    {
        {
            constexpr T q1(1.0f, 0.0f, 0.0f, 0.0f);
            constexpr T res = TRAP::Math::Inverse(q1);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(res, T(1.0f, 0.0f, 0.0f, 0.0f), Epsilon)));
        }
        {
            constexpr T q1(0.5f, 0.5f, 0.5f, 0.5f);
            constexpr T res = TRAP::Math::Inverse(q1);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(res, T(0.5f, -0.5f, -0.5f, -0.5f), Epsilon)));
        }
        {
            constexpr T q1(2.0f, 1.0f, 0.5f, 0.3f);
            constexpr T res = TRAP::Math::Inverse(q1);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(res, T(0.374532f, -0.187266f, -0.093633f, -0.056180f), typename T::valueType(0.000001f))));
        }
        {
            constexpr T q1(-0.5f, 0.2f, -0.7f, -0.9f);
            constexpr T res = TRAP::Math::Inverse(q1);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(res, T(-0.314465f, -0.125786f, 0.440252f, 0.566038f), typename T::valueType(0.000001f))));
        }
    }
}

template<typename T>
requires ((TRAP::Math::IsMat3<T> || TRAP::Math::IsMat4<T> || TRAP::Math::IsQuat<T>) && std::floating_point<typename T::valueType>)
void RunInverseEdgeTests()
{
    constexpr typename T::valueType Epsilon = std::numeric_limits<typename T::valueType>::epsilon();

    constexpr typename T::valueType min = std::numeric_limits<typename T::valueType>::lowest();
    constexpr typename T::valueType max = std::numeric_limits<typename T::valueType>::max();
    constexpr typename T::valueType inf = std::numeric_limits<typename T::valueType>::infinity();
    constexpr typename T::valueType ninf = -std::numeric_limits<typename T::valueType>::infinity();
    constexpr typename T::valueType nan = std::numeric_limits<typename T::valueType>::quiet_NaN();

    if constexpr(TRAP::Math::IsMat<T>)
    {
        {
            constexpr T a(0.0f);
            constexpr T b(TRAP::Math::tMat4<typename T::valueType>{nan});
            REQUIRE(TRAP::Math::All(TRAP::Math::Not(TRAP::Math::Equal(TRAP::Math::Inverse(a), b))));
        }
        {
            constexpr T a(TRAP::Math::tMat4<typename T::valueType>{min});
            constexpr T b(TRAP::Math::tMat4<typename T::valueType>{nan});
            REQUIRE(TRAP::Math::All(TRAP::Math::Not(TRAP::Math::Equal(TRAP::Math::Inverse(a), b))));
        }
        {
            constexpr T a(TRAP::Math::tMat4<typename T::valueType>{max});
            constexpr T b(TRAP::Math::tMat4<typename T::valueType>{nan});
            REQUIRE(TRAP::Math::All(TRAP::Math::Not(TRAP::Math::Equal(TRAP::Math::Inverse(a), b))));
        }
        {
            constexpr T a(TRAP::Math::tMat4<typename T::valueType>{inf});
            constexpr T b(TRAP::Math::tMat4<typename T::valueType>{nan});
            REQUIRE(TRAP::Math::All(TRAP::Math::Not(TRAP::Math::Equal(TRAP::Math::Inverse(a), b))));
        }
        {
            constexpr T a(TRAP::Math::tMat4<typename T::valueType>{ninf});
            constexpr T b(TRAP::Math::tMat4<typename T::valueType>{nan});
            REQUIRE(TRAP::Math::All(TRAP::Math::Not(TRAP::Math::Equal(TRAP::Math::Inverse(a), b))));
        }
        {
            constexpr T a(TRAP::Math::tMat4<typename T::valueType>{nan});
            REQUIRE(TRAP::Math::All(TRAP::Math::Not(TRAP::Math::Equal(TRAP::Math::Inverse(a), a))));
        }
    }
    else if constexpr(TRAP::Math::IsQuat<T>)
    {
        {
            constexpr T q1(min, min, min, min);
            const T res = TRAP::Math::Inverse(q1);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(res, T(0.0f, 0.0f, 0.0f, 0.0f), Epsilon)));
        }
        {
            constexpr T q1(max, max, max, max);
            const T res = TRAP::Math::Inverse(q1);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(res, T(0.0f, 0.0f, 0.0f, 0.0f), Epsilon)));
        }
        {
            constexpr T q1(inf, inf, inf, inf);
            const T res = TRAP::Math::Inverse(q1);
            REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(res)));
        }
        {
            constexpr T q1(ninf, ninf, ninf, ninf);
            const T res = TRAP::Math::Inverse(q1);
            REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(res)));
        }
        {
            constexpr T q1(nan, nan, nan, nan);
            const T res = TRAP::Math::Inverse(q1);
            REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(res)));
        }
    }
}

TEST_CASE("TRAP::Math::Inverse()", "[math][generic][inverse]")
{
    SECTION("Mat3 - double")
    {
        RunCompileTimeInverseTests<TRAP::Math::Mat3d>();
        RunInverseEdgeTests<TRAP::Math::Mat3d>();
    }
    SECTION("Mat3 - float")
    {
        RunCompileTimeInverseTests<TRAP::Math::Mat3f>();
        RunInverseEdgeTests<TRAP::Math::Mat3f>();
    }

    SECTION("Mat4 - double")
    {
        RunCompileTimeInverseTests<TRAP::Math::Mat4d>();
        RunInverseEdgeTests<TRAP::Math::Mat4d>();
    }
    SECTION("Mat4 - float")
    {
        RunCompileTimeInverseTests<TRAP::Math::Mat4f>();
        RunInverseEdgeTests<TRAP::Math::Mat4f>();
    }

    SECTION("Quat - double")
    {
        RunCompileTimeInverseTests<TRAP::Math::Quatd>();
        RunInverseEdgeTests<TRAP::Math::Quatd>();
    }
    SECTION("Quat - float")
    {
        RunCompileTimeInverseTests<TRAP::Math::Quatf>();
        RunInverseEdgeTests<TRAP::Math::Quatf>();
    }
}
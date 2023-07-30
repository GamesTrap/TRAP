#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>
#include <array>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::floating_point<T>
void RunLogTests()
{
    constexpr std::array<std::tuple<T, T>, 4> values
    {
        std::tuple(T(0.5f), T(-0.69314718055994529f)),
        std::tuple(T(1.0f), T(0.0f)),
        std::tuple(T(1.5f), T(0.405465f)),
        std::tuple(T(41.5f), T(3.72569346f))
    };

    for(const auto& [x, expected] : values)
    {
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Log(x), expected, T(0.000001f)));
    }
}

template<typename T>
requires TRAP::Math::IsVec<T> && std::floating_point<typename T::valueType>
void RunLogVecTests()
{
    REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Log(T(TRAP::Math::e<typename T::valueType>())), T(1.0f), T(0.01f))));
}

template<typename T>
requires TRAP::Math::IsQuat<T>
void RunLogQuatTests()
{
    constexpr typename T::valueType Epsilon = typename T::valueType(0.001f);

    const T q(TRAP::Math::Vec<3, typename T::valueType>(1.0f, 0.0f, 0.0f), TRAP::Math::Vec<3, typename T::valueType>(0.0f, 1.0f, 0.0f));
    const T p = TRAP::Math::Log(q);
    REQUIRE(TRAP::Math::Any(TRAP::Math::NotEqual(q, p, Epsilon)));
}

template<typename T>
requires std::floating_point<T>
void RunLogEdgeTests()
{
    constexpr T nan = std::numeric_limits<T>::quiet_NaN();
    constexpr T inf = std::numeric_limits<T>::infinity();
    constexpr T ninf = -std::numeric_limits<T>::infinity();
    constexpr T min = std::numeric_limits<T>::lowest();

    REQUIRE(TRAP::Math::IsInf(TRAP::Math::Log(T(0.0f))));
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Log(min)));
    REQUIRE(TRAP::Math::IsInf(TRAP::Math::Log(inf)));
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Log(ninf)));
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Log(nan)));
}

TEST_CASE("TRAP::Math::Log()", "[math][generic][log]")
{
    SECTION("Scalar - double")
    {
        RunLogTests<double>();
        RunLogEdgeTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunLogTests<float>();
        RunLogEdgeTests<float>();
    }

    SECTION("Vec2 - double")
    {
        RunLogVecTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunLogVecTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - double")
    {
        RunLogVecTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunLogVecTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - double")
    {
        RunLogVecTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunLogVecTests<TRAP::Math::Vec4f>();
    }

    SECTION("Quat - double")
    {
        RunLogQuatTests<TRAP::Math::Quatd>();
    }
    SECTION("Quat - float")
    {
        RunLogQuatTests<TRAP::Math::Quatf>();
    }
}
#include <concepts>
#include <numeric>
#include <limits>
#include <cmath>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::floating_point<T>
void RunCosTests()
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    constexpr std::array<T, 8> values
    {
        T(-1.5), T(0.0), T(0.001), T(1.001), T(1.5), T(11.1), T(50.0), T(150.0)
    };

    for(const T val : values)
    {
        REQUIRE_THAT(TRAP::Math::Cos(val), Catch::Matchers::WithinRel(std::cos(val), Epsilon));
    }
}

template<typename T>
requires std::floating_point<T>
void RunCosEdgeTests()
{
    constexpr T nan = std::numeric_limits<T>::quiet_NaN();
    constexpr T inf = std::numeric_limits<T>::infinity();
    constexpr T ninf = -std::numeric_limits<T>::infinity();

    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Cos(nan)));
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Cos(inf)));
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Cos(ninf)));
}

template<typename T>
requires TRAP::Math::IsVec<T> && std::floating_point<typename T::valueType>
void RunCosVecTests()
{
    constexpr typename T::valueType Epsilon = std::numeric_limits<typename T::valueType>::epsilon();

    constexpr std::array<T, 8> values
    {
        T(typename T::valueType(-1.5)), T(typename T::valueType(0.0)), T(typename T::valueType(0.001)), T(typename T::valueType(1.001)),
        T(typename T::valueType(1.5)), T(typename T::valueType(11.1)), T(typename T::valueType(50.0)), T(typename T::valueType(150.0))
    };

    for(const T val : values)
    {
        const T s = TRAP::Math::Cos(val);
        for(uint32_t l = 0; l < val.Length(); ++l)
            REQUIRE(TRAP::Math::Equal(s[l], std::cos(val[l]), Epsilon));
    }
}

template<typename T>
requires TRAP::Math::IsVec<T> && std::floating_point<typename T::valueType>
void RunCosVecEdgeTests()
{
    constexpr typename T::valueType nan = std::numeric_limits<typename T::valueType>::quiet_NaN();
    constexpr typename T::valueType inf = std::numeric_limits<typename T::valueType>::infinity();
    constexpr typename T::valueType ninf = -std::numeric_limits<typename T::valueType>::infinity();

    REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::Cos(T(nan)))));
    REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::Cos(T(inf)))));
    REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::Cos(T(ninf)))));
}

TEST_CASE("TRAP::Math::Cos()", "[math][generic][cos]")
{
    SECTION("Scalar - double")
    {
        RunCosTests<double>();
        RunCosEdgeTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunCosTests<float>();
        RunCosEdgeTests<float>();
    }

    SECTION("Vec2 - double")
    {
        RunCosVecTests<TRAP::Math::Vec2d>();
        RunCosVecEdgeTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunCosVecTests<TRAP::Math::Vec2f>();
        RunCosVecEdgeTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - double")
    {
        RunCosVecTests<TRAP::Math::Vec3d>();
        RunCosVecEdgeTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunCosVecTests<TRAP::Math::Vec3f>();
        RunCosVecEdgeTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - double")
    {
        RunCosVecTests<TRAP::Math::Vec4d>();
        RunCosVecEdgeTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunCosVecTests<TRAP::Math::Vec4f>();
        RunCosVecEdgeTests<TRAP::Math::Vec4f>();
    }
}

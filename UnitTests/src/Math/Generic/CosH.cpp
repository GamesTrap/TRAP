#include <concepts>
#include <numeric>
#include <limits>
#include <cmath>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::floating_point<T>
void RunCosHTests()
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    constexpr std::array<T, 6> values
    {
        T(-0.5), T(0.0), T(0.001), T(0.5), T(1.0), T(5.0)
    };

    for(const T val : values)
    {
        REQUIRE_THAT(TRAP::Math::CosH(val), Catch::Matchers::WithinRel(std::cosh(val), Epsilon));
    }
}

template<typename T>
requires std::floating_point<T>
void RunCosHEdgeTests()
{
    constexpr T nan = std::numeric_limits<T>::quiet_NaN();

    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::CosH(nan)));
}

template<typename T>
requires TRAP::Math::IsVec<T> && std::floating_point<typename T::valueType>
void RunCosHVecTests()
{
    constexpr typename T::valueType Epsilon = std::numeric_limits<typename T::valueType>::epsilon();

    constexpr std::array<T, 8> values
    {
        T(typename T::valueType(-0.5)), T(typename T::valueType(0.0)), T(typename T::valueType(0.001)),
        T(typename T::valueType(0.5)), T(typename T::valueType(1.0)), T(typename T::valueType(5.0))
    };

    for(const T val : values)
    {
        const T s = TRAP::Math::CosH(val);
        for(uint32_t l = 0; l < val.Length(); ++l)
            REQUIRE(TRAP::Math::Equal(s[l], std::cosh(val[l]), Epsilon));
    }
}

template<typename T>
requires TRAP::Math::IsVec<T> && std::floating_point<typename T::valueType>
void RunCosHVecEdgeTests()
{
    constexpr typename T::valueType nan = std::numeric_limits<typename T::valueType>::quiet_NaN();

    REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::CosH(T(nan)))));
}

TEST_CASE("TRAP::Math::CosH()", "[math][generic][cosh]")
{
    SECTION("Scalar - double")
    {
        RunCosHTests<double>();
        RunCosHEdgeTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunCosHTests<float>();
        RunCosHEdgeTests<float>();
    }

    SECTION("Vec2 - double")
    {
        RunCosHVecTests<TRAP::Math::Vec2d>();
        RunCosHVecEdgeTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunCosHVecTests<TRAP::Math::Vec2f>();
        RunCosHVecEdgeTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - double")
    {
        RunCosHVecTests<TRAP::Math::Vec3d>();
        RunCosHVecEdgeTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunCosHVecTests<TRAP::Math::Vec3f>();
        RunCosHVecEdgeTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - double")
    {
        RunCosHVecTests<TRAP::Math::Vec4d>();
        RunCosHVecEdgeTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunCosHVecTests<TRAP::Math::Vec4f>();
        RunCosHVecEdgeTests<TRAP::Math::Vec4f>();
    }
}

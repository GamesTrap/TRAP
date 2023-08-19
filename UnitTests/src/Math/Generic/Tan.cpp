#include <concepts>
#include <numeric>
#include <limits>
#include <cmath>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::floating_point<T>
void RunTanTests()
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    constexpr std::array<T, 8> values
    {
        T(-1.5), T(0.0), T(0.001), T(1.001), T(1.5), T(11.1), T(50.0), T(150.0)
    };

    for(const T val : values)
    {
        REQUIRE_THAT(TRAP::Math::Tan(val), Catch::Matchers::WithinRel(std::tan(val), Epsilon));
    }
}

template<typename T>
requires std::floating_point<T>
void RunTanEdgeTests()
{
    constexpr T nan = std::numeric_limits<T>::quiet_NaN();
    constexpr T inf = std::numeric_limits<T>::infinity();
    constexpr T ninf = -std::numeric_limits<T>::infinity();

    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Tan(nan)));
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Tan(inf)));
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Tan(ninf)));
}

template<typename T>
requires TRAP::Math::IsVec<T> && std::floating_point<typename T::valueType>
void RunTanVecTests()
{
    constexpr typename T::valueType Epsilon = std::numeric_limits<typename T::valueType>::epsilon();

    constexpr std::array<T, 8> values
    {
        T(typename T::valueType(-1.5)), T(typename T::valueType(0.0)), T(typename T::valueType(0.001)), T(typename T::valueType(1.001)),
        T(typename T::valueType(1.5)), T(typename T::valueType(11.1)), T(typename T::valueType(50.0)), T(typename T::valueType(150.0))
    };

    for(const T val : values)
    {
        const T s = TRAP::Math::Tan(val);
        for(uint32_t l = 0; l < val.Length(); ++l)
            REQUIRE(TRAP::Math::Equal(s[l], std::tan(val[l]), Epsilon));
    }
}

template<typename T>
requires TRAP::Math::IsVec<T> && std::floating_point<typename T::valueType>
void RunTanVecEdgeTests()
{
    constexpr typename T::valueType nan = std::numeric_limits<typename T::valueType>::quiet_NaN();
    constexpr typename T::valueType inf = std::numeric_limits<typename T::valueType>::infinity();
    constexpr typename T::valueType ninf = -std::numeric_limits<typename T::valueType>::infinity();

    REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::Tan(T(nan)))));
    REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::Tan(T(inf)))));
    REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::Tan(T(ninf)))));
}

TEST_CASE("TRAP::Math::Tan()", "[math][generic][tan]")
{
    SECTION("Scalar - double")
    {
        RunTanTests<double>();
        RunTanEdgeTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunTanTests<float>();
        RunTanEdgeTests<float>();
    }

    SECTION("Vec2 - double")
    {
        RunTanVecTests<TRAP::Math::Vec2d>();
        RunTanVecEdgeTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunTanVecTests<TRAP::Math::Vec2f>();
        RunTanVecEdgeTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - double")
    {
        RunTanVecTests<TRAP::Math::Vec3d>();
        RunTanVecEdgeTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunTanVecTests<TRAP::Math::Vec3f>();
        RunTanVecEdgeTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - double")
    {
        RunTanVecTests<TRAP::Math::Vec4d>();
        RunTanVecEdgeTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunTanVecTests<TRAP::Math::Vec4f>();
        RunTanVecEdgeTests<TRAP::Math::Vec4f>();
    }
}

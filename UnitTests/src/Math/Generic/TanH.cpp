#include <concepts>
#include <numeric>
#include <limits>
#include <cmath>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::floating_point<T>
void RunTanHTests()
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    constexpr std::array<T, 6> values
    {
        T(-0.5), T(0.0), T(0.001), T(0.5), T(1.0), T(5.0)
    };

    for(const T val : values)
    {
        REQUIRE_THAT(TRAP::Math::TanH(val), Catch::Matchers::WithinRel(std::tanh(val), Epsilon));
    }
}

template<typename T>
requires std::floating_point<T>
void RunTanHEdgeTests()
{
    constexpr T nan = std::numeric_limits<T>::quiet_NaN();

    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::TanH(nan)));
}

template<typename T>
requires TRAP::Math::IsVec<T> && std::floating_point<typename T::valueType>
void RunTanHVecTests()
{
    constexpr typename T::valueType Epsilon = std::numeric_limits<typename T::valueType>::epsilon();

    constexpr std::array<T, 8> values
    {
        T(typename T::valueType(-0.5)), T(typename T::valueType(0.0)), T(typename T::valueType(0.001)),
        T(typename T::valueType(0.5)), T(typename T::valueType(1.0)), T(typename T::valueType(5.0))
    };

    for(const T val : values)
    {
        const T s = TRAP::Math::TanH(val);
        for(uint32_t l = 0; l < val.Length(); ++l)
            REQUIRE(TRAP::Math::Equal(s[l], std::tanh(val[l]), Epsilon));
    }
}

template<typename T>
requires TRAP::Math::IsVec<T> && std::floating_point<typename T::valueType>
void RunTanHVecEdgeTests()
{
    constexpr typename T::valueType nan = std::numeric_limits<typename T::valueType>::quiet_NaN();

    REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::TanH(T(nan)))));
}

TEST_CASE("TRAP::Math::TanH()", "[math][generic][tanh]")
{
    SECTION("Scalar - double")
    {
        RunTanHTests<double>();
        RunTanHEdgeTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunTanHTests<float>();
        RunTanHEdgeTests<float>();
    }

    SECTION("Vec2 - double")
    {
        RunTanHVecTests<TRAP::Math::Vec2d>();
        RunTanHVecEdgeTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunTanHVecTests<TRAP::Math::Vec2f>();
        RunTanHVecEdgeTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - double")
    {
        RunTanHVecTests<TRAP::Math::Vec3d>();
        RunTanHVecEdgeTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunTanHVecTests<TRAP::Math::Vec3f>();
        RunTanHVecEdgeTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - double")
    {
        RunTanHVecTests<TRAP::Math::Vec4d>();
        RunTanHVecEdgeTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunTanHVecTests<TRAP::Math::Vec4f>();
        RunTanHVecEdgeTests<TRAP::Math::Vec4f>();
    }
}

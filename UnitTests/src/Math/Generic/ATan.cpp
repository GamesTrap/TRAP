#include <concepts>
#include <numeric>
#include <limits>
#include <cmath>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::floating_point<T>
void RunATanTests()
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    constexpr std::array<T, 23> values
    {
        T(-1.5), T(-0.5), T(0.0), T(0.001), T(0.49), T(0.99), T(1.001), T(1.49), T(1.5),
        T(1.99), T(2.49), T(2.51), T(3.99), T(7.0), T(11.0), T(11.1), T(25.0), T(50.0),
        T(101.0), T(150.0), T(900.0), T(1001.0), T(1000000.0)
    };

    for(const T val : values)
    {
        REQUIRE_THAT(TRAP::Math::ATan(val), Catch::Matchers::WithinRel(std::atan(val), Epsilon));
    }
}

template<typename T>
requires std::floating_point<T>
void RunATanEdgeTests()
{
    constexpr T nan = std::numeric_limits<T>::quiet_NaN();

    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::ATan(nan)));
}

template<typename T>
requires TRAP::Math::IsVec<T> && std::floating_point<typename T::valueType>
void RunATanVecTests()
{
    constexpr typename T::valueType Epsilon = std::numeric_limits<typename T::valueType>::epsilon();

    constexpr std::array<T, 23> values
    {
        T(typename T::valueType(-1.5)),  T(typename T::valueType(-0.5)),  T(typename T::valueType(0.0)),   T(typename T::valueType(0.001)),
        T(typename T::valueType(0.49)), T(typename T::valueType(0.99)), T(typename T::valueType(1.001)), T(typename T::valueType(1.49)),
        T(typename T::valueType(1.5)), T(typename T::valueType(1.99)),  T(typename T::valueType(2.49)),  T(typename T::valueType(2.51)),
        T(typename T::valueType(3.99)),   T(typename T::valueType(7.0)),  T(typename T::valueType(11.0)), T(typename T::valueType(11.1)),
        T(typename T::valueType(25.0)), T(typename T::valueType(50.0)), T(typename T::valueType(101.0)), T(typename T::valueType(150.0)),
        T(typename T::valueType(900.0)), T(typename T::valueType(1001.0)), T(typename T::valueType(1000000.0))
    };

    for(const T val : values)
    {
        const T s = TRAP::Math::ATan(val);
        for(uint32_t l = 0; l < val.Length(); ++l)
            REQUIRE(TRAP::Math::Equal(s[l], std::atan(val[l]), Epsilon));
    }
}

template<typename T>
requires TRAP::Math::IsVec<T> && std::floating_point<typename T::valueType>
void RunATanVecEdgeTests()
{
    constexpr typename T::valueType nan = std::numeric_limits<typename T::valueType>::quiet_NaN();

    REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::ATan(T(nan)))));
}

TEST_CASE("TRAP::Math::ATan(x)", "[math][generic][atan]")
{
    SECTION("Scalar - double")
    {
        RunATanTests<double>();
        RunATanEdgeTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunATanTests<float>();
        RunATanEdgeTests<float>();
    }

    SECTION("Vec2 - double")
    {
        RunATanVecTests<TRAP::Math::Vec2d>();
        RunATanVecEdgeTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunATanVecTests<TRAP::Math::Vec2f>();
        RunATanVecEdgeTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - double")
    {
        RunATanVecTests<TRAP::Math::Vec3d>();
        RunATanVecEdgeTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunATanVecTests<TRAP::Math::Vec3f>();
        RunATanVecEdgeTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - double")
    {
        RunATanVecTests<TRAP::Math::Vec4d>();
        RunATanVecEdgeTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunATanVecTests<TRAP::Math::Vec4f>();
        RunATanVecEdgeTests<TRAP::Math::Vec4f>();
    }
}

#include <concepts>
#include <numeric>
#include <limits>
#include <cmath>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::floating_point<T>
void RunATanHTests()
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    constexpr std::array<T, 3> values
    {
        T(-0.99), T(0.0), T(0.001)
    };

    for(const T val : values)
    {
        REQUIRE_THAT(TRAP::Math::ATanH(val), Catch::Matchers::WithinRel(std::atanh(val), Epsilon));
    }
}

template<typename T>
requires std::floating_point<T>
void RunATanHEdgeTests()
{
    constexpr T nan = std::numeric_limits<T>::quiet_NaN();

    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::ATanH(nan)));
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::ATanH(T(1.1))));
    REQUIRE(TRAP::Math::IsInf(TRAP::Math::ATanH(T(-1.0))));
    REQUIRE(TRAP::Math::IsInf(TRAP::Math::ATanH(T(1.0))));
}

template<typename T>
requires TRAP::Math::IsVec<T> && std::floating_point<typename T::valueType>
void RunATanHVecTests()
{
    constexpr typename T::valueType Epsilon = std::numeric_limits<typename T::valueType>::epsilon();

    constexpr std::array<T, 3> values
    {
        T(typename T::valueType(-0.99)), T(typename T::valueType(0.0)), T(typename T::valueType(0.001))
    };

    for(const T val : values)
    {
        const T s = TRAP::Math::ATanH(val);
        for(uint32_t l = 0; l < val.Length(); ++l)
            REQUIRE(TRAP::Math::Equal(s[l], std::atanh(val[l]), Epsilon));
    }
}

template<typename T>
requires TRAP::Math::IsVec<T> && std::floating_point<typename T::valueType>
void RunATanHVecEdgeTests()
{
    constexpr typename T::valueType nan = std::numeric_limits<typename T::valueType>::quiet_NaN();

    REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::ATanH(T(nan)))));
    REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::ATanH(T(typename T::valueType(1.1))))));
    REQUIRE(TRAP::Math::All(TRAP::Math::IsInf(TRAP::Math::ATanH(T(typename T::valueType(-1.0))))));
    REQUIRE(TRAP::Math::All(TRAP::Math::IsInf(TRAP::Math::ATanH(T(typename T::valueType(1.0))))));
}

TEST_CASE("TRAP::Math::ATanH()", "[math][generic][atanh]")
{
    SECTION("Scalar - double")
    {
        RunATanHTests<double>();
        RunATanHEdgeTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunATanHTests<float>();
        RunATanHEdgeTests<float>();
    }

    SECTION("Vec2 - double")
    {
        RunATanHVecTests<TRAP::Math::Vec2d>();
        RunATanHVecEdgeTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunATanHVecTests<TRAP::Math::Vec2f>();
        RunATanHVecEdgeTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - double")
    {
        RunATanHVecTests<TRAP::Math::Vec3d>();
        RunATanHVecEdgeTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunATanHVecTests<TRAP::Math::Vec3f>();
        RunATanHVecEdgeTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - double")
    {
        RunATanHVecTests<TRAP::Math::Vec4d>();
        RunATanHVecEdgeTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunATanHVecTests<TRAP::Math::Vec4f>();
        RunATanHVecEdgeTests<TRAP::Math::Vec4f>();
    }
}

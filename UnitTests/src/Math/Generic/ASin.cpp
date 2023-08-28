#include <concepts>
#include <numeric>
#include <limits>
#include <cmath>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::floating_point<T>
void RunASinTests()
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    constexpr std::array<T, 5> values
    {
        T(-0.5), T(0.0), T(0.001), T(0.5), T(1.0)
    };

    for(const T val : values)
    {
        REQUIRE_THAT(TRAP::Math::ASin(val), Catch::Matchers::WithinRel(std::asin(val), Epsilon));
    }
}

template<typename T>
requires std::floating_point<T>
void RunASinEdgeTests()
{
    constexpr T nan = std::numeric_limits<T>::quiet_NaN();
    constexpr T inf = std::numeric_limits<T>::infinity();
    constexpr T ninf = -std::numeric_limits<T>::infinity();

    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::ASin(nan)));
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::ASin(inf)));
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::ASin(ninf)));
}

template<typename T>
requires TRAP::Math::IsVec<T> && std::floating_point<typename T::valueType>
void RunASinVecTests()
{
    constexpr typename T::valueType Epsilon = std::numeric_limits<typename T::valueType>::epsilon();

    constexpr std::array<T, 5> values
    {
        T(typename T::valueType(-0.5)), T(typename T::valueType(0.0)), T(typename T::valueType(0.001)),
        T(typename T::valueType(0.5)), T(typename T::valueType(1.0))
    };

    for(const T val : values)
    {
        const T s = TRAP::Math::ASin(val);
        for(uint32_t l = 0; l < val.Length(); ++l)
            REQUIRE(TRAP::Math::Equal(s[l], std::asin(val[l]), Epsilon));
    }
}

template<typename T>
requires TRAP::Math::IsVec<T> && std::floating_point<typename T::valueType>
void RunASinVecEdgeTests()
{
    constexpr typename T::valueType nan = std::numeric_limits<typename T::valueType>::quiet_NaN();
    constexpr typename T::valueType inf = std::numeric_limits<typename T::valueType>::infinity();
    constexpr typename T::valueType ninf = -std::numeric_limits<typename T::valueType>::infinity();

    REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::ASin(T(nan)))));
    REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::ASin(T(inf)))));
    REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::ASin(T(ninf)))));
}

TEST_CASE("TRAP::Math::ASin()", "[math][generic][asin]")
{
    SECTION("Scalar - double")
    {
        RunASinTests<double>();
        RunASinEdgeTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunASinTests<float>();
        RunASinEdgeTests<float>();
    }

    SECTION("Vec2 - double")
    {
        RunASinVecTests<TRAP::Math::Vec2d>();
        RunASinVecEdgeTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunASinVecTests<TRAP::Math::Vec2f>();
        RunASinVecEdgeTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - double")
    {
        RunASinVecTests<TRAP::Math::Vec3d>();
        RunASinVecEdgeTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunASinVecTests<TRAP::Math::Vec3f>();
        RunASinVecEdgeTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - double")
    {
        RunASinVecTests<TRAP::Math::Vec4d>();
        RunASinVecEdgeTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunASinVecTests<TRAP::Math::Vec4f>();
        RunASinVecEdgeTests<TRAP::Math::Vec4f>();
    }
}
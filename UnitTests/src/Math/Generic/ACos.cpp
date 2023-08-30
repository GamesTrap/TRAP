#include <concepts>
#include <numeric>
#include <limits>
#include <cmath>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::floating_point<T>
void RunACosTests()
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    constexpr std::array<T, 5> values
    {
        T(-0.5), T(0.0), T(0.001), T(0.5), T(1.0)
    };

    for(const T val : values)
    {
        REQUIRE_THAT(TRAP::Math::ACos(val), Catch::Matchers::WithinRel(std::acos(val), Epsilon));
    }
}

template<typename T>
requires std::floating_point<T>
void RunACosEdgeTests()
{
    constexpr T nan = std::numeric_limits<T>::quiet_NaN();
    constexpr T inf = std::numeric_limits<T>::infinity();
    constexpr T ninf = -std::numeric_limits<T>::infinity();

    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::ACos(nan)));
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::ACos(inf)));
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::ACos(ninf)));
}

template<typename T>
requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
void RunACosVecTests()
{
    constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

    constexpr std::array<T, 5> values
    {
        T(typename T::value_type(-0.5)), T(typename T::value_type(0.0)), T(typename T::value_type(0.001)),
        T(typename T::value_type(0.5)), T(typename T::value_type(1.0))
    };

    for(const T val : values)
    {
        const T s = TRAP::Math::ACos(val);
        for(uint32_t l = 0; l < val.Length(); ++l)
            REQUIRE(TRAP::Math::Equal(s[l], std::acos(val[l]), Epsilon));
    }
}

template<typename T>
requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
void RunACosVecEdgeTests()
{
    constexpr typename T::value_type nan = std::numeric_limits<typename T::value_type>::quiet_NaN();
    constexpr typename T::value_type inf = std::numeric_limits<typename T::value_type>::infinity();
    constexpr typename T::value_type ninf = -std::numeric_limits<typename T::value_type>::infinity();

    REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::ACos(T(nan)))));
    REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::ACos(T(inf)))));
    REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::ACos(T(ninf)))));
}

TEST_CASE("TRAP::Math::ACos()", "[math][generic][acos]")
{
    SECTION("Scalar - double")
    {
        RunACosTests<double>();
        RunACosEdgeTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunACosTests<float>();
        RunACosEdgeTests<float>();
    }

    SECTION("Vec2 - double")
    {
        RunACosVecTests<TRAP::Math::Vec2d>();
        RunACosVecEdgeTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunACosVecTests<TRAP::Math::Vec2f>();
        RunACosVecEdgeTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - double")
    {
        RunACosVecTests<TRAP::Math::Vec3d>();
        RunACosVecEdgeTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunACosVecTests<TRAP::Math::Vec3f>();
        RunACosVecEdgeTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - double")
    {
        RunACosVecTests<TRAP::Math::Vec4d>();
        RunACosVecEdgeTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunACosVecTests<TRAP::Math::Vec4f>();
        RunACosVecEdgeTests<TRAP::Math::Vec4f>();
    }
}

#include <concepts>
#include <numeric>
#include <limits>
#include <cmath>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::floating_point<T>
void RunSinTests()
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    constexpr std::array<T, 8> values
    {
        T(-1.5), T(0.0), T(0.001), T(1.001), T(1.5), T(11.1), T(50.0), T(150.0)
    };

    for(const T val : values)
    {
        REQUIRE_THAT(TRAP::Math::Sin(val), Catch::Matchers::WithinRel(std::sin(val), Epsilon));
    }
}

template<typename T>
requires std::floating_point<T>
void RunSinEdgeTests()
{
    constexpr T nan = std::numeric_limits<T>::quiet_NaN();
    constexpr T inf = std::numeric_limits<T>::infinity();
    constexpr T ninf = -std::numeric_limits<T>::infinity();

    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Sin(nan)));
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Sin(inf)));
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Sin(ninf)));
}

template<typename T>
requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
void RunSinVecTests()
{
    constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

    constexpr std::array<T, 8> values
    {
        T(typename T::value_type(-1.5)), T(typename T::value_type(0.0)), T(typename T::value_type(0.001)), T(typename T::value_type(1.001)),
        T(typename T::value_type(1.5)), T(typename T::value_type(11.1)), T(typename T::value_type(50.0)), T(typename T::value_type(150.0))
    };

    for(const T val : values)
    {
        const T s = TRAP::Math::Sin(val);
        for(uint32_t l = 0; l < val.Length(); ++l)
            REQUIRE(TRAP::Math::Equal(s[l], std::sin(val[l]), Epsilon));
    }
}

template<typename T>
requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
void RunSinVecEdgeTests()
{
    constexpr typename T::value_type nan = std::numeric_limits<typename T::value_type>::quiet_NaN();
    constexpr typename T::value_type inf = std::numeric_limits<typename T::value_type>::infinity();
    constexpr typename T::value_type ninf = -std::numeric_limits<typename T::value_type>::infinity();

    REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::Sin(T(nan)))));
    REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::Sin(T(inf)))));
    REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::Sin(T(ninf)))));
}

TEST_CASE("TRAP::Math::Sin()", "[math][generic][sin]")
{
    SECTION("Scalar - double")
    {
        RunSinTests<double>();
        RunSinEdgeTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunSinTests<float>();
        RunSinEdgeTests<float>();
    }

    SECTION("Vec2 - double")
    {
        RunSinVecTests<TRAP::Math::Vec2d>();
        RunSinVecEdgeTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunSinVecTests<TRAP::Math::Vec2f>();
        RunSinVecEdgeTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - double")
    {
        RunSinVecTests<TRAP::Math::Vec3d>();
        RunSinVecEdgeTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunSinVecTests<TRAP::Math::Vec3f>();
        RunSinVecEdgeTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - double")
    {
        RunSinVecTests<TRAP::Math::Vec4d>();
        RunSinVecEdgeTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunSinVecTests<TRAP::Math::Vec4f>();
        RunSinVecEdgeTests<TRAP::Math::Vec4f>();
    }
}

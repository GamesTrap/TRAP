#include <concepts>
#include <numeric>
#include <limits>
#include <cmath>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::floating_point<T>
void RunSinHTests()
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    constexpr std::array<T, 6> values
    {
        T(-0.5), T(0.0), T(0.001), T(0.5), T(1.0), T(5.0)
    };

    for(const T val : values)
    {
        REQUIRE_THAT(TRAP::Math::SinH(val), Catch::Matchers::WithinRel(std::sinh(val), Epsilon));
    }
}

template<typename T>
requires std::floating_point<T>
void RunSinHEdgeTests()
{
    constexpr T nan = std::numeric_limits<T>::quiet_NaN();

    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::SinH(nan)));
}

template<typename T>
requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
void RunSinHVecTests()
{
    constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

    constexpr std::array<T, 8> values
    {
        T(typename T::value_type(-0.5)), T(typename T::value_type(0.0)), T(typename T::value_type(0.001)),
        T(typename T::value_type(0.5)), T(typename T::value_type(1.0)), T(typename T::value_type(5.0))
    };

    for(const T val : values)
    {
        const T s = TRAP::Math::SinH(val);
        for(uint32_t l = 0; l < val.Length(); ++l)
            REQUIRE(TRAP::Math::Equal(s[l], std::sinh(val[l]), Epsilon));
    }
}

template<typename T>
requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
void RunSinHVecEdgeTests()
{
    constexpr typename T::value_type nan = std::numeric_limits<typename T::value_type>::quiet_NaN();

    REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::SinH(T(nan)))));
}

TEST_CASE("TRAP::Math::SinH()", "[math][generic][sinh]")
{
    SECTION("Scalar - double")
    {
        RunSinHTests<double>();
        RunSinHEdgeTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunSinHTests<float>();
        RunSinHEdgeTests<float>();
    }

    SECTION("Vec2 - double")
    {
        RunSinHVecTests<TRAP::Math::Vec2d>();
        RunSinHVecEdgeTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunSinHVecTests<TRAP::Math::Vec2f>();
        RunSinHVecEdgeTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - double")
    {
        RunSinHVecTests<TRAP::Math::Vec3d>();
        RunSinHVecEdgeTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunSinHVecTests<TRAP::Math::Vec3f>();
        RunSinHVecEdgeTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - double")
    {
        RunSinHVecTests<TRAP::Math::Vec4d>();
        RunSinHVecEdgeTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunSinHVecTests<TRAP::Math::Vec4f>();
        RunSinHVecEdgeTests<TRAP::Math::Vec4f>();
    }
}

#include <concepts>
#include <numeric>
#include <limits>
#include <cmath>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::floating_point<T>
void RunASinHTests()
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    constexpr std::array<T, 7> values
    {
        T(-1.5), T(0.0), T(0.001), T(1.001), T(1.5), T(11.1), T(50.0)
    };

    for(const T val : values)
    {
        REQUIRE_THAT(TRAP::Math::ASinH(val), Catch::Matchers::WithinRel(std::asinh(val), Epsilon));
    }
}

template<typename T>
requires std::floating_point<T>
void RunASinHEdgeTests()
{
    constexpr T nan = std::numeric_limits<T>::quiet_NaN();

    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::ASinH(nan)));
}

template<typename T>
requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
void RunASinHVecTests()
{
    constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

    constexpr std::array<T, 7> values
    {
        T(typename T::value_type(-1.5)), T(typename T::value_type(0.0)), T(typename T::value_type(0.001)),
        T(typename T::value_type(1.001)), T(typename T::value_type(1.5)), T(typename T::value_type(11.1)),
        T(typename T::value_type(50.0))
    };

    for(const T val : values)
    {
        const T s = TRAP::Math::ASinH(val);
        for(uint32_t l = 0; l < val.Length(); ++l)
            REQUIRE(TRAP::Math::Equal(s[l], std::asinh(val[l]), Epsilon));
    }
}

template<typename T>
requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
void RunASinHVecEdgeTests()
{
    constexpr typename T::value_type nan = std::numeric_limits<typename T::value_type>::quiet_NaN();

    REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::ASinH(T(nan)))));
}

TEST_CASE("TRAP::Math::ASinH()", "[math][generic][asinh]")
{
    SECTION("Scalar - double")
    {
        RunASinHTests<double>();
        RunASinHEdgeTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunASinHTests<float>();
        RunASinHEdgeTests<float>();
    }

    SECTION("Vec2 - double")
    {
        RunASinHVecTests<TRAP::Math::Vec2d>();
        RunASinHVecEdgeTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunASinHVecTests<TRAP::Math::Vec2f>();
        RunASinHVecEdgeTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - double")
    {
        RunASinHVecTests<TRAP::Math::Vec3d>();
        RunASinHVecEdgeTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunASinHVecTests<TRAP::Math::Vec3f>();
        RunASinHVecEdgeTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - double")
    {
        RunASinHVecTests<TRAP::Math::Vec4d>();
        RunASinHVecEdgeTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunASinHVecTests<TRAP::Math::Vec4f>();
        RunASinHVecEdgeTests<TRAP::Math::Vec4f>();
    }
}

#include <concepts>
#include <numeric>
#include <limits>
#include <cmath>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::floating_point<T>
void RunACosHTests()
{
    static constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    static constexpr std::array<T, 4> values
    {
        T(1.001), T(1.5), T(11.1), T(50.0)
    };

    for(const T val : values)
    {
        REQUIRE_THAT(TRAP::Math::ACosH(val), Catch::Matchers::WithinRel(std::acosh(val), Epsilon));
    }
}

template<typename T>
requires std::floating_point<T>
void RunACosHEdgeTests()
{
    static constexpr T nan = std::numeric_limits<T>::quiet_NaN();

    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::ACosH(nan)));
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::ACosH(T(0.5))));
}

template<typename T>
requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
void RunACosHVecTests()
{
    static constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

    static constexpr std::array<T, 4> values
    {
        T(typename T::value_type(1.001)), T(typename T::value_type(1.5)),
        T(typename T::value_type(11.1)), T(typename T::value_type(50.0))
    };

    for(const T val : values)
    {
        const T s = TRAP::Math::ACosH(val);
        for(uint32_t l = 0; l < val.Length(); ++l)
            REQUIRE(TRAP::Math::Equal(s[l], std::acosh(val[l]), Epsilon));
    }
}

template<typename T>
requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
void RunACosHVecEdgeTests()
{
    static constexpr typename T::value_type nan = std::numeric_limits<typename T::value_type>::quiet_NaN();

    REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::ACosH(T(nan)))));
    REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::ACosH(T(typename T::value_type(0.5))))));
}

TEST_CASE("TRAP::Math::ACosH()", "[math][generic][acosh]")
{
    SECTION("Scalar - double")
    {
        RunACosHTests<double>();
        RunACosHEdgeTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunACosHTests<float>();
        RunACosHEdgeTests<float>();
    }

    SECTION("Vec2 - double")
    {
        RunACosHVecTests<TRAP::Math::Vec2d>();
        RunACosHVecEdgeTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunACosHVecTests<TRAP::Math::Vec2f>();
        RunACosHVecEdgeTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - double")
    {
        RunACosHVecTests<TRAP::Math::Vec3d>();
        RunACosHVecEdgeTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunACosHVecTests<TRAP::Math::Vec3f>();
        RunACosHVecEdgeTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - double")
    {
        RunACosHVecTests<TRAP::Math::Vec4d>();
        RunACosHVecEdgeTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunACosHVecTests<TRAP::Math::Vec4f>();
        RunACosHVecEdgeTests<TRAP::Math::Vec4f>();
    }
}

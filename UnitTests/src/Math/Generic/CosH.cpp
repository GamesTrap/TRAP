#include <concepts>
#include <numeric>
#include <limits>
#include <cmath>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::floating_point<T>
consteval void RunCosHCompileTimeTests()
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    static_assert(TRAP::Math::Equal(TRAP::Math::CosH(T(-0.5)), T(1.12763), T(0.00001f)));
    static_assert(TRAP::Math::Equal(TRAP::Math::CosH(T(0.0)), T(1.0), Epsilon));
    static_assert(TRAP::Math::Equal(TRAP::Math::CosH(T(0.001)), T(1.0), T(0.000001f)));
    static_assert(TRAP::Math::Equal(TRAP::Math::CosH(T(0.5)), T(1.12763), T(0.00001f)));
    static_assert(TRAP::Math::Equal(TRAP::Math::CosH(T(1.0)), T(1.54308), T(0.000001f)));
    static_assert(TRAP::Math::Equal(TRAP::Math::CosH(T(5.0)), T(74.2099), T(0.0001f)));
}

template<typename T>
requires std::floating_point<T>
void RunCosHRunTimeTests()
{
    static constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    static constexpr std::array<T, 6> values
    {
        T(-0.5), T(0.0), T(0.001), T(0.5), T(1.0), T(5.0)
    };

    for(const T val : values)
    {
        REQUIRE_THAT(TRAP::Math::CosH(val), Catch::Matchers::WithinRel(std::cosh(val), Epsilon));
    }
}

template<typename T>
requires std::floating_point<T>
void RunCosHEdgeTests()
{
    static constexpr T nan = std::numeric_limits<T>::quiet_NaN();

    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::CosH(nan)));
}

template<typename T>
requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
consteval void RunCosHVecCompileTimeTests()
{
    constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

    static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::CosH(T(typename T::value_type(-0.5))), T(1.12763), T(0.00001f))));
    static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::CosH(T(typename T::value_type(0.0))), T(1.0), Epsilon)));
    static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::CosH(T(typename T::value_type(0.001))), T(1.0), T(0.000001f))));
    static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::CosH(T(typename T::value_type(0.5))), T(1.12763), T(0.00001f))));
    static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::CosH(T(typename T::value_type(1.0))), T(1.54308), T(0.000001f))));
    static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::CosH(T(typename T::value_type(5.0))), T(74.2099), T(0.0001f))));
}

template<typename T>
requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
void RunCosHVecRunTimeTests()
{
    static constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

    static constexpr std::array<T, 6> values
    {
        T(typename T::value_type(-0.5)), T(typename T::value_type(0.0)), T(typename T::value_type(0.001)),
        T(typename T::value_type(0.5)), T(typename T::value_type(1.0)), T(typename T::value_type(5.0))
    };

    for(const T val : values)
    {
        const T s = TRAP::Math::CosH(val);
        for(uint32_t l = 0; l < val.Length(); ++l)
            REQUIRE(TRAP::Math::Equal(s[l], std::cosh(val[l]), Epsilon));
    }
}

template<typename T>
requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
void RunCosHVecEdgeTests()
{
    static constexpr typename T::value_type nan = std::numeric_limits<typename T::value_type>::quiet_NaN();

    REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::CosH(T(nan)))));
}

TEST_CASE("TRAP::Math::CosH()", "[math][generic][cosh]")
{
    SECTION("Scalar - double")
    {
        RunCosHRunTimeTests<double>();
        RunCosHCompileTimeTests<double>();
        RunCosHEdgeTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunCosHRunTimeTests<float>();
        RunCosHCompileTimeTests<float>();
        RunCosHEdgeTests<float>();
    }

    SECTION("Vec2 - double")
    {
        RunCosHVecRunTimeTests<TRAP::Math::Vec2d>();
        RunCosHVecCompileTimeTests<TRAP::Math::Vec2d>();
        RunCosHVecEdgeTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunCosHVecRunTimeTests<TRAP::Math::Vec2f>();
        RunCosHVecCompileTimeTests<TRAP::Math::Vec2f>();
        RunCosHVecEdgeTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - double")
    {
        RunCosHVecRunTimeTests<TRAP::Math::Vec3d>();
        RunCosHVecCompileTimeTests<TRAP::Math::Vec3d>();
        RunCosHVecEdgeTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunCosHVecRunTimeTests<TRAP::Math::Vec3f>();
        RunCosHVecCompileTimeTests<TRAP::Math::Vec3f>();
        RunCosHVecEdgeTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - double")
    {
        RunCosHVecRunTimeTests<TRAP::Math::Vec4d>();
        RunCosHVecCompileTimeTests<TRAP::Math::Vec4d>();
        RunCosHVecEdgeTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunCosHVecRunTimeTests<TRAP::Math::Vec4f>();
        RunCosHVecCompileTimeTests<TRAP::Math::Vec4f>();
        RunCosHVecEdgeTests<TRAP::Math::Vec4f>();
    }
}

#include <concepts>
#include <numeric>
#include <limits>
#include <cmath>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::floating_point<T>
consteval void RunACosCompileTimeTests()
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    static_assert(TRAP::Math::Equal(TRAP::Math::ACos(T(-0.5)), T(2.0944), T(0.00001f)));
    static_assert(TRAP::Math::Equal(TRAP::Math::ACos(T(0.0)), T(1.5708), T(0.00001f)));
    static_assert(TRAP::Math::Equal(TRAP::Math::ACos(T(0.001)), T(1.5698), T(0.00001f)));
    static_assert(TRAP::Math::Equal(TRAP::Math::ACos(T(0.5)), T(1.0472), T(0.00001f)));
    static_assert(TRAP::Math::Equal(TRAP::Math::ACos(T(1.0)), T(0.0), Epsilon));
}

template<typename T>
requires std::floating_point<T>
void RunACosRunTimeTests()
{
    static constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    static constexpr std::array<T, 5> values
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
    static constexpr T nan = std::numeric_limits<T>::quiet_NaN();
    static constexpr T inf = std::numeric_limits<T>::infinity();
    static constexpr T ninf = -std::numeric_limits<T>::infinity();

    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::ACos(nan)));
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::ACos(inf)));
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::ACos(ninf)));
}

template<typename T>
requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
consteval void RunACosVecCompileTimeTests()
{
    constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

    static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ACos(T(typename T::value_type(-0.5f))), T(2.0944f), T(0.00001f))));
    static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ACos(T(typename T::value_type(0.0f))), T(1.5708f), T(0.00001f))));
    static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ACos(T(typename T::value_type(0.001f))), T(1.5698f), T(0.00001f))));
    static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ACos(T(typename T::value_type(0.5f))), T(1.0472f), T(0.00001f))));
    static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ACos(T(typename T::value_type(1.0f))), T(0.0f), Epsilon)));
}

template<typename T>
requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
void RunACosVecRunTimeTests()
{
    static constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

    static constexpr std::array<T, 5> values
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
    static constexpr typename T::value_type nan = std::numeric_limits<typename T::value_type>::quiet_NaN();
    static constexpr typename T::value_type inf = std::numeric_limits<typename T::value_type>::infinity();
    static constexpr typename T::value_type ninf = -std::numeric_limits<typename T::value_type>::infinity();

    REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::ACos(T(nan)))));
    REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::ACos(T(inf)))));
    REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::ACos(T(ninf)))));
}

TEST_CASE("TRAP::Math::ACos()", "[math][generic][acos]")
{
    SECTION("Scalar - double")
    {
        RunACosRunTimeTests<double>();
        RunACosCompileTimeTests<double>();
        RunACosEdgeTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunACosRunTimeTests<float>();
        RunACosCompileTimeTests<float>();
        RunACosEdgeTests<float>();
    }

    SECTION("Vec2 - double")
    {
        RunACosVecRunTimeTests<TRAP::Math::Vec2d>();
        RunACosVecCompileTimeTests<TRAP::Math::Vec2d>();
        RunACosVecEdgeTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunACosVecRunTimeTests<TRAP::Math::Vec2f>();
        RunACosVecCompileTimeTests<TRAP::Math::Vec2f>();
        RunACosVecEdgeTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - double")
    {
        RunACosVecRunTimeTests<TRAP::Math::Vec3d>();
        RunACosVecCompileTimeTests<TRAP::Math::Vec3d>();
        RunACosVecEdgeTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunACosVecRunTimeTests<TRAP::Math::Vec3f>();
        RunACosVecCompileTimeTests<TRAP::Math::Vec3f>();
        RunACosVecEdgeTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - double")
    {
        RunACosVecRunTimeTests<TRAP::Math::Vec4d>();
        RunACosVecCompileTimeTests<TRAP::Math::Vec4d>();
        RunACosVecEdgeTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunACosVecRunTimeTests<TRAP::Math::Vec4f>();
        RunACosVecCompileTimeTests<TRAP::Math::Vec4f>();
        RunACosVecEdgeTests<TRAP::Math::Vec4f>();
    }
}

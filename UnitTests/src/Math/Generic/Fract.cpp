#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::floating_point<T>
consteval void RunFractCompileTimeTests()
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    static_assert(TRAP::Math::Equal(TRAP::Math::Fract(T(1.5)), T(0.5), Epsilon));
}

template<typename T>
requires (TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>)
consteval void RunFractVecCompileTimeTests()
{
    constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

    static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Fract(T(TRAP::Math::Vec<4, typename T::value_type>(1.1, 1.2, 1.5, 1.7))), T(TRAP::Math::Vec<4, typename T::value_type>(0.1, 0.2, 0.5, 0.7)), Epsilon)));
}

template<typename T>
requires std::floating_point<T> || (TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>)
void RunFractRunTimeTests()
{
    if constexpr(std::floating_point<T>)
    {
        static constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        static constexpr std::array<std::pair<T, T>, 1> values
        {
            std::pair{T(1.5), T(0.5)}
        };

        for(const auto& [val, expected] : values)
        {
            REQUIRE_THAT(TRAP::Math::Fract(val), Catch::Matchers::WithinRel(expected, Epsilon));
        }
    }
    else if constexpr(TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>)
    {
        static constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

        static constexpr std::array<std::pair<T, T>, 1> values
        {
            std::pair<T, T>{TRAP::Math::Vec<4, typename T::value_type>(1.1, 1.2, 1.5, 1.7),
                            TRAP::Math::Vec<4, typename T::value_type>(0.1, 0.2, 0.5, 0.7)},
        };

        for(const auto& [val, expected] : values)
        {
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Fract(val), expected, Epsilon)));
        }
    }
}

template<typename T>
requires std::floating_point<T>
void RunFractEdgeRunTimeTests()
{
    static constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    static constexpr T max = std::numeric_limits<T>::max();
    REQUIRE(TRAP::Math::Equal(TRAP::Math::Fract(max), T(0.0), Epsilon));
    static constexpr T min = std::numeric_limits<T>::lowest();
    REQUIRE(TRAP::Math::Equal(TRAP::Math::Fract(min), T(0.0), Epsilon));
    static constexpr T nan = std::numeric_limits<T>::quiet_NaN();
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Fract(nan)));
    static constexpr T inf = std::numeric_limits<T>::infinity();
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Fract(inf)));
    static constexpr T ninf = -std::numeric_limits<T>::infinity();
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Fract(ninf)));
}

TEST_CASE("TRAP::Math::Fract()", "[math][generic][fract]")
{
    SECTION("Scalar - double")
    {
        RunFractRunTimeTests<double>();
        RunFractEdgeRunTimeTests<double>();
        RunFractCompileTimeTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunFractRunTimeTests<float>();
        RunFractEdgeRunTimeTests<float>();
        RunFractCompileTimeTests<float>();
    }

    SECTION("Vec2 - double")
    {
        RunFractRunTimeTests<TRAP::Math::Vec2d>();
        RunFractVecCompileTimeTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunFractRunTimeTests<TRAP::Math::Vec2f>();
        RunFractVecCompileTimeTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - double")
    {
        RunFractRunTimeTests<TRAP::Math::Vec3d>();
        RunFractVecCompileTimeTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunFractRunTimeTests<TRAP::Math::Vec3f>();
        RunFractVecCompileTimeTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - double")
    {
        RunFractRunTimeTests<TRAP::Math::Vec4d>();
        RunFractVecCompileTimeTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunFractRunTimeTests<TRAP::Math::Vec4f>();
        RunFractVecCompileTimeTests<TRAP::Math::Vec4f>();
    }
}
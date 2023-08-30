#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>
#include <array>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::floating_point<T>
void RunFModTests()
{
    constexpr std::array<std::tuple<T, T, T>, 10> values
    {
        std::tuple(T( 5.3f), T( 2.0f), T( 1.3f)),
        std::tuple(T(-5.3f), T( 2.0f), T(-1.3f)),
        std::tuple(T( 5.3f), T(-2.0f), T( 1.3f)),
        std::tuple(T(-5.3f), T(-2.0f), T(-1.3f)),
        std::tuple(T( 18.5f), T( 4.2f), T( 1.7f)),
        std::tuple(T(-18.5f), T( 4.2f), T(-1.7f)),
        std::tuple(T( 18.5f), T(-4.2f), T( 1.7f)),
        std::tuple(T(-18.5f), T(-4.2f), T(-1.7f)),
        std::tuple(T(  3.0f), T(  2.0f), T(1.0f)),
        std::tuple(T( 22.0f), T(-10.0f), T(2.0f)),
    };

    for(const auto& [x, y, expected] : values)
    {
        const T res = TRAP::Math::FMod(x, y);
        REQUIRE(TRAP::Math::Equal(res, expected, T(0.00001f)));
    }

    //http://stackoverflow.com/questions/7610631/glsl-mod-vs-hlsl-fmod
    for(T y = T(-10.0f); y < T(10.0f); y += T(0.1f))
    {
        for(T x = T(-10.0f); x < T(10.0f); x += T(0.1f))
        {
            const T A = std::fmod(x, y);
            // const T B = std::remainder(x, y);
            const T C = TRAP::Math::FMod(x, y);
            const T D = (x - y * TRAP::Math::Trunc(x / y));

            // REQUIRE(TRAP::Math::Equal(A, B, T(0.0001f)));
            REQUIRE(TRAP::Math::Equal(A, C, T(0.0001f)));
            REQUIRE(TRAP::Math::Equal(A, D, T(0.00001f)));
        }
    }
}

template<typename T>
requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
void RunFModVecTests()
{
    constexpr std::array<std::tuple<typename T::value_type, typename T::value_type, typename T::value_type>, 10> values
    {
        std::tuple(typename T::value_type( 5.3f), typename T::value_type( 2.0f), typename T::value_type( 1.3f)),
        std::tuple(typename T::value_type(-5.3f), typename T::value_type( 2.0f), typename T::value_type(-1.3f)),
        std::tuple(typename T::value_type( 5.3f), typename T::value_type(-2.0f), typename T::value_type( 1.3f)),
        std::tuple(typename T::value_type(-5.3f), typename T::value_type(-2.0f), typename T::value_type(-1.3f)),
        std::tuple(typename T::value_type( 18.5f), typename T::value_type( 4.2f), typename T::value_type( 1.7f)),
        std::tuple(typename T::value_type(-18.5f), typename T::value_type( 4.2f), typename T::value_type(-1.7f)),
        std::tuple(typename T::value_type( 18.5f), typename T::value_type(-4.2f), typename T::value_type( 1.7f)),
        std::tuple(typename T::value_type(-18.5f), typename T::value_type(-4.2f), typename T::value_type(-1.7f)),
        std::tuple(typename T::value_type(  3.0f), typename T::value_type(  2.0f), typename T::value_type(1.0f)),
        std::tuple(typename T::value_type( 22.0f), typename T::value_type(-10.0f), typename T::value_type(2.0f)),
    };

    for(const auto& [x, y, expected] : values)
    {
        const T res = TRAP::Math::FMod(T(x), y);
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(res, T(expected), typename T::value_type(0.00001f))));

        const T res1 = TRAP::Math::FMod(T(x), T(y));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(res1, T(expected), T(0.00001f))));
    }
}

template<typename T>
requires std::floating_point<T>
void RunFModEdgeTests()
{
    constexpr T inf = std::numeric_limits<T>::infinity();
    constexpr T ninf = -std::numeric_limits<T>::infinity();
    constexpr T nan = std::numeric_limits<T>::quiet_NaN();

    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::FMod(inf, T( 0.0f))));
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::FMod(inf, T(-1.0f))));
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::FMod(inf, T( 1.0f))));
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::FMod(inf, inf)));
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::FMod(inf, ninf)));
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::FMod(inf, nan)));

    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::FMod(ninf, T( 0.0f))));
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::FMod(ninf, T(-1.0f))));
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::FMod(ninf, T( 1.0f))));
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::FMod(ninf, inf)));
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::FMod(ninf, ninf)));
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::FMod(ninf, nan)));

    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::FMod(nan, T( 0.0f))));
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::FMod(nan, T(-1.0f))));
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::FMod(nan, T( 1.0f))));
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::FMod(nan, inf)));
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::FMod(nan, ninf)));
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::FMod(nan, nan)));

    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::FMod(T( 0.0f), T( 0.0f))));
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::FMod(T(-0.0f), T( 0.0f))));
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::FMod(T( 0.0f), T(-0.0f))));
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::FMod(T(-0.0f), T(-0.0f))));
}

TEST_CASE("TRAP::Math::FMod()", "[math][generic][fmod]")
{
    SECTION("Scalar - double")
    {
        RunFModTests<double>();
        RunFModEdgeTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunFModTests<float>();
        RunFModEdgeTests<float>();
    }

    SECTION("Vec2 - double")
    {
        RunFModVecTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunFModVecTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - double")
    {
        RunFModVecTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunFModVecTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - double")
    {
        RunFModVecTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunFModVecTests<TRAP::Math::Vec4f>();
    }
}
#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::floating_point<T> || (TRAP::Math::IsVec<T> && std::floating_point<typename T::valueType>)
void RunCeilTests(const T val)
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();
    if constexpr(std::floating_point<T>)
    {
        REQUIRE_THAT(TRAP::Math::Ceil(val), Catch::Matchers::WithinRel(std::ceil(val), Epsilon));
    }
    else if constexpr(TRAP::Math::IsVec<T>)
    {
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Ceil(val), T(std::ceil(val[0])), Epsilon)));
    }
}

TEST_CASE("TRAP::Math::Ceil()", "[math][generic][ceil]")
{
    SECTION("Edge cases")
    {
        constexpr float Epsilon = std::numeric_limits<float>::epsilon();
        constexpr float fmax = std::numeric_limits<float>::max();
        constexpr long double ninf = -std::numeric_limits<long double>::infinity();
        constexpr long double inf = std::numeric_limits<long double>::infinity();
        constexpr long double nan = std::numeric_limits<long double>::quiet_NaN();

        REQUIRE_THAT(TRAP::Math::Ceil(fmax), Catch::Matchers::WithinRel(std::ceil(fmax), Epsilon));
        REQUIRE_THAT(TRAP::Math::Ceil(ninf), Catch::Matchers::WithinRel(std::ceil(ninf), Epsilon));
        REQUIRE_THAT(TRAP::Math::Ceil(inf), Catch::Matchers::WithinRel(std::ceil(inf), Epsilon));
        REQUIRE((TRAP::Math::IsNaN(TRAP::Math::Ceil(nan)) && std::isnan(std::ceil(nan))));
    }

    const double val = GENERATE(values(
        {
            0.0, -0.0, 0.1, -0.1, 0.5, -0.5, 0.9, -0.9, 1.0, -1.0,
            1.5, -1.5, 1.9, -1.9, 4.2, 4.5, 4.7, 5.0, -4.2, -4.5,
            -4.7, -5.0, 42e32, -42e32
        }));

    SECTION("Scalar - double")
    {
        RunCeilTests(val);
    }
    SECTION("Scalar - float")
    {
        RunCeilTests(static_cast<float>(val));
    }

    SECTION("Vec2 - double")
    {
        RunCeilTests(TRAP::Math::Vec2d(static_cast<double>(val)));
    }
    SECTION("Vec2 - float")
    {
        RunCeilTests(TRAP::Math::Vec2f(static_cast<float>(val)));
    }

    SECTION("Vec3 - double")
    {
        RunCeilTests(TRAP::Math::Vec3d(static_cast<double>(val)));
    }
    SECTION("Vec3 - float")
    {
        RunCeilTests(TRAP::Math::Vec3f(static_cast<float>(val)));
    }

    SECTION("Vec4 - double")
    {
        RunCeilTests(TRAP::Math::Vec4d(static_cast<double>(val)));
    }
    SECTION("Vec4 - float")
    {
        RunCeilTests(TRAP::Math::Vec4f(static_cast<float>(val)));
    }
}
#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::floating_point<T> || (TRAP::Math::IsVec<T> && std::floating_point<typename T::valueType>)
void RunTruncTests(const T val)
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();
    if constexpr(std::floating_point<T>)
    {
        REQUIRE_THAT(TRAP::Math::Trunc(val), Catch::Matchers::WithinRel(std::trunc(val), Epsilon));
    }
    else if constexpr(TRAP::Math::IsVec<T>)
    {
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Trunc(val), T(std::trunc(val[0])), Epsilon)));
    }
}

TEST_CASE("TRAP::Math::Trunc()", "[math][generic][trunc]")
{
    SECTION("Edge cases")
    {
        constexpr float Epsilon = std::numeric_limits<float>::epsilon();
        constexpr float fmax = std::numeric_limits<float>::max();
        constexpr long double ninf = -std::numeric_limits<long double>::infinity();
        constexpr long double inf = std::numeric_limits<long double>::infinity();
        constexpr long double nan = std::numeric_limits<long double>::quiet_NaN();

        REQUIRE_THAT(TRAP::Math::Trunc(fmax), Catch::Matchers::WithinRel(std::trunc(fmax), Epsilon));
        REQUIRE_THAT(TRAP::Math::Trunc(ninf), Catch::Matchers::WithinRel(std::trunc(ninf), Epsilon));
        REQUIRE_THAT(TRAP::Math::Trunc(inf), Catch::Matchers::WithinRel(std::trunc(inf), Epsilon));
        REQUIRE((TRAP::Math::IsNaN(TRAP::Math::Trunc(nan)) && std::isnan(std::trunc(nan))));
    }

    const double val = GENERATE(values(
        {
            0.0, -0.0, 0.1, -0.1, 0.5, -0.5, 0.9, -0.9, 1.0, -1.0,
            1.5, -1.5, 1.9, -1.9, 4.2, 4.5, 4.7, 5.0, -4.2, -4.5,
            -4.7, -5.0, 42e32, -42e32
        }));

    SECTION("Scalar - double")
    {
        RunTruncTests(val);
    }
    SECTION("Scalar - float")
    {
        RunTruncTests(static_cast<float>(val));
    }

    SECTION("Vec2 - double")
    {
        RunTruncTests(TRAP::Math::Vec2d(static_cast<double>(val)));
    }
    SECTION("Vec2 - float")
    {
        RunTruncTests(TRAP::Math::Vec2f(static_cast<float>(val)));
    }

    SECTION("Vec3 - double")
    {
        RunTruncTests(TRAP::Math::Vec3d(static_cast<double>(val)));
    }
    SECTION("Vec3 - float")
    {
        RunTruncTests(TRAP::Math::Vec3f(static_cast<float>(val)));
    }

    SECTION("Vec4 - double")
    {
        RunTruncTests(TRAP::Math::Vec4d(static_cast<double>(val)));
    }
    SECTION("Vec4 - float")
    {
        RunTruncTests(TRAP::Math::Vec4f(static_cast<float>(val)));
    }
}
#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::floating_point<T> || (TRAP::Math::IsVec<T> && std::floating_point<typename T::valueType>)
void RunFloorTests(const T val)
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();
    if constexpr(std::floating_point<T>)
    {
        REQUIRE_THAT(TRAP::Math::Floor(val), Catch::Matchers::WithinRel(std::floor(val), Epsilon));
    }
    else if constexpr(TRAP::Math::IsVec<T>)
    {
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Floor(val), T(std::floor(val[0])), Epsilon)));
    }
}

TEST_CASE("TRAP::Math::Floor()", "[math][generic][floor]")
{
    SECTION("Edge cases")
    {
        constexpr float Epsilon = std::numeric_limits<float>::epsilon();
        constexpr float fmax = std::numeric_limits<float>::max();
        constexpr long double ninf = -std::numeric_limits<long double>::infinity();
        constexpr long double inf = std::numeric_limits<long double>::infinity();
        constexpr long double nan = std::numeric_limits<long double>::quiet_NaN();

        REQUIRE_THAT(TRAP::Math::Floor(fmax), Catch::Matchers::WithinRel(std::floor(fmax), Epsilon));
        REQUIRE_THAT(TRAP::Math::Floor(ninf), Catch::Matchers::WithinRel(std::floor(ninf), Epsilon));
        REQUIRE_THAT(TRAP::Math::Floor(inf), Catch::Matchers::WithinRel(std::floor(inf), Epsilon));
        REQUIRE((TRAP::Math::IsNaN(TRAP::Math::Floor(nan)) && std::isnan(std::floor(nan))));
    }

    const double val = GENERATE(values(
        {
            0.0, -0.0, 0.1, -0.1, 0.5, -0.5, 0.9, -0.9, 1.0, -1.0, 1.1,
            1.5, -1.5, 1.9, -1.9, 4.2, 4.5, 4.7, 5.0, -4.2, -4.5,
            -4.7, -5.0, 42e32, -42e32
        }));

    SECTION("Scalar - double")
    {
        RunFloorTests(val);
    }
    SECTION("Scalar - float")
    {
        RunFloorTests(static_cast<float>(val));
    }

    SECTION("Vec2 - double")
    {
        RunFloorTests(TRAP::Math::Vec2d(static_cast<double>(val)));
    }
    SECTION("Vec2 - float")
    {
        RunFloorTests(TRAP::Math::Vec2f(static_cast<float>(val)));
    }

    SECTION("Vec3 - double")
    {
        RunFloorTests(TRAP::Math::Vec3d(static_cast<double>(val)));
    }
    SECTION("Vec3 - float")
    {
        RunFloorTests(TRAP::Math::Vec3f(static_cast<float>(val)));
    }

    SECTION("Vec4 - double")
    {
        RunFloorTests(TRAP::Math::Vec4d(static_cast<double>(val)));
    }
    SECTION("Vec4 - float")
    {
        RunFloorTests(TRAP::Math::Vec4f(static_cast<float>(val)));
    }
}
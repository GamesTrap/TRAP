#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>
#include <array>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::floating_point<T>
void RunAngleAxisTests()
{
    const TRAP::Math::tQuat<T> a = TRAP::Math::AngleAxis(T(0.0f), TRAP::Math::tVec3<T>(0.0f, 0.0f, 1.0f));
    const TRAP::Math::tQuat<T> b = TRAP::Math::AngleAxis(TRAP::Math::PI<T>() * T(0.5f), TRAP::Math::tVec3<T>(0.0f, 0.0f, 1.0f));
    const TRAP::Math::tQuat<T> c = TRAP::Math::Mix(a, b, T(0.5f));
    const TRAP::Math::tQuat<T> d = TRAP::Math::AngleAxis(TRAP::Math::PI<T>() * T(0.25f), TRAP::Math::tVec3<T>(0.0f, 0.0f, 1.0f));

    REQUIRE(TRAP::Math::Equal(c.x(), d.x(), T(0.01f)));
    REQUIRE(TRAP::Math::Equal(c.y(), d.y(), T(0.01f)));
    REQUIRE(TRAP::Math::Equal(c.z(), d.z(), T(0.01f)));
    REQUIRE(TRAP::Math::Equal(c.w(), d.w(), T(0.01f)));
}

template<typename T>
requires std::floating_point<T>
void RunAngleAxisEdgeTests()
{
    constexpr T inf = std::numeric_limits<T>::infinity();
    constexpr T ninf = -std::numeric_limits<T>::infinity();
    constexpr T nan = std::numeric_limits<T>::quiet_NaN();

    {
        const TRAP::Math::tQuat<T> res = TRAP::Math::AngleAxis(inf, TRAP::Math::tVec3<T>(0.0f, 0.0f, 1.0f));
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(res)));
    }
    {
        const TRAP::Math::tQuat<T> res = TRAP::Math::AngleAxis(ninf, TRAP::Math::tVec3<T>(1.0f, 1.0f, 0.0f));
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(res)));
    }
    {
        const TRAP::Math::tQuat<T> res = TRAP::Math::AngleAxis(nan, TRAP::Math::tVec3<T>(0.0f, 1.0f, 1.0f));
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(res)));
    }
}

TEST_CASE("TRAP::Math::AngleAxis()", "[math][generic][angleaxis]")
{
    SECTION("Scalar - double")
    {
        RunAngleAxisTests<double>();
        RunAngleAxisEdgeTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunAngleAxisTests<float>();
        RunAngleAxisEdgeTests<float>();
    }
}
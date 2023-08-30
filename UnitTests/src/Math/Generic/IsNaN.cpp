#include <cmath>
#include <numeric>
#include <limits>
#include <concepts>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires (std::floating_point<T> ||
          (TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>) ||
          (TRAP::Math::IsQuat<T> && std::floating_point<typename T::value_type>))
consteval void RunCompileTimeIsNaNTest()
{
    if constexpr(std::floating_point<T>)
    {
        constexpr T nan = std::numeric_limits<T>::quiet_NaN();
        static_assert(TRAP::Math::IsNaN(nan));
        static_assert(TRAP::Math::IsNaN(-nan));
    }
    else if constexpr(TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>)
    {
        constexpr typename T::value_type nan = std::numeric_limits<typename T::value_type>::quiet_NaN();
        static_assert(TRAP::Math::Any(TRAP::Math::IsNaN(T(nan))));
        static_assert(TRAP::Math::Any(TRAP::Math::IsNaN(T(-nan))));
    }
    else if constexpr(TRAP::Math::IsQuat<T> && std::floating_point<typename T::value_type>)
    {
        constexpr typename T::value_type nan = std::numeric_limits<typename T::value_type>::quiet_NaN();
        static_assert(TRAP::Math::Any(TRAP::Math::IsNaN(T(nan, nan, nan, nan))));
        static_assert(TRAP::Math::Any(TRAP::Math::IsNaN(T(-nan, -nan, -nan, -nan))));
    }
}

TEST_CASE("TRAP::Math::IsNaN()", "[math][generic][isnan]")
{
    SECTION("Scalar - double")
    {
        RunCompileTimeIsNaNTest<double>();
    }
    SECTION("Scalar - float")
    {
        RunCompileTimeIsNaNTest<float>();
    }

    SECTION("Vec2 - double")
    {
        RunCompileTimeIsNaNTest<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunCompileTimeIsNaNTest<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - double")
    {
        RunCompileTimeIsNaNTest<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunCompileTimeIsNaNTest<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - double")
    {
        RunCompileTimeIsNaNTest<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunCompileTimeIsNaNTest<TRAP::Math::Vec4f>();
    }

    SECTION("Quat - double")
    {
        RunCompileTimeIsNaNTest<TRAP::Math::Quatd>();
    }
    SECTION("Quat - float")
    {
        RunCompileTimeIsNaNTest<TRAP::Math::Quatf>();
    }
}